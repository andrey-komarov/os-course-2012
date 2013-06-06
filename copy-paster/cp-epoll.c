#include <stropts.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/epoll.h>

#include <stdio.h>

#define BUFSIZE (1 << 13)

typedef struct epoll_event epoll_event;

int main(int argc, char** argv)
{
    if (argc % 2 == 0 || argc < 2)
    {
        printf("usage: cp-epoll <f1> <t1> <f2> <t2> ...\n");
        return 1;
    }
    int n = argc / 2;

    epoll_event* events = (epoll_event*)malloc(2 * n * sizeof(epoll_event));
    epoll_event* revents = (epoll_event*)malloc(2 * n * sizeof(epoll_event));
    int* fds = (int*)malloc(2 * n * sizeof(int));
    int i;
    for (i = 0; i < 2 * n; i++)
        fds[i] = atoi(argv[i + 1]);

    int epollfd = epoll_create(2 * n);
    if (epollfd < 0)
    {
        perror("epoll_create");
        exit(1);
    }
    for (i = 0; i < n; i++)
    {
        events[2 * i].data.u32 = 2 * i;
        events[2 * i].events = EPOLLIN;
        events[2 * i + 1].data.u32 = 2 * i + 1;
        events[2 * i + 1].events = 0;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fds[i], &events[2 * i]);
    }

    char** bufs = (char**)malloc(n * sizeof(char*));
    for (i = 0; i < n; i++)
        bufs[i] = (char*)malloc(BUFSIZE);
    int* bufpos = (int*)malloc(n * sizeof(int));
    memset(bufpos, 0, n * sizeof(int));

    int* done = (int*)malloc(n * sizeof(int));
    memset(done, 0, n * sizeof(int));

    while (n)
    {
        int nfds = epoll_wait(epollfd, revents, 2 * n, -1);
        //printf("nfds = %d\n", nfds);
        if (nfds < 0)
        {
            perror("epoll_wait");
            exit(1);
        }
        int i;
        for (i = 0; i < nfds; i++)
        {
            if (revents[i].events & EPOLLIN)
            {
                int me = revents[i].data.u32;
                int dual = me ^ 1;
                int id = me / 2;
                int freesp = BUFSIZE - bufpos[i];
                int rd = read(fds[me], bufs[id], freesp);
                //printf("r %d from %d\n", rd, fds[me]);
                if (rd == 0)
                {
                    n--;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, fds[me], &events[me]);
                    done[id] = 1;
                }
                bufpos[i] += rd;
                if (bufpos[i] == BUFSIZE && !done[id])
                {
                    //printf("disabling reading from %d\n", fds[me]);
                    events[me].events &= ~EPOLLIN;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, fds[me], &events[me]);
                }
                if (bufpos[i] != 0 && !(events[dual].events & EPOLLOUT))
                {
                    //printf("enabling writing to %d\n", fds[dual]);
                    events[dual].events |= EPOLLOUT;
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, fds[dual], &events[dual]);
                }
            }
            if (revents[i].events & EPOLLOUT)
            {
                int me = revents[i].data.u32;
                int dual = me ^ 1;
                int id = me / 2;
                int w = write(fds[dual], bufs[id], bufpos[id]);
                //printf("w %d to %d\n", w, fds[me]);
                memmove(bufs[id], bufs[id] + w, w);
                bufpos[id] -= w;
                if (bufpos[id] < BUFSIZE && !(events[dual].events & EPOLLIN))
                {
                    //printf("enabling reading from %d\n", fds[dual]);
                    events[dual].events |= EPOLLIN;
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, fds[dual], &events[dual]);
                }
                if (bufpos[id] == 0)
                {
                    //printf("disabling writing to %d\n", fds[me]);
                    events[me].events &= ~EPOLLOUT;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, fds[me], &events[me]);
                }
            }
        }
    }

    return 0;
}
