#include <stropts.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

#define BUFSIZE 1024

typedef struct pollfd pollfd;

int main(int argc, char** argv)
{
    if (argc % 2 == 0)
    {
        printf("usage: cp-poll <f1> <t1> <f2> <t2> ...\n");
        return 1;
    }
    int n = argc / 2;
    
    pollfd* fds = (pollfd*)malloc(2 * n * sizeof(pollfd));
    int i;
    for (i = 0; i < n; i++)
    {
        fds[2 * i].fd = atoi(argv[2 * i]); // TODO check `atoi` result
        fds[2 * i + 1].fd = atoi(argv[2 * i + 1]); // TODO same
        fds[2 * i].events = POLLIN;
        fds[2 * i + 1].events = POLLOUT;
    }

    char** bufs = (char**)malloc(n * sizeof(char*));
    for (i = 0; i < n; i++)
        bufs[i] = (char*)malloc(BUFSIZE);
    int* bufpos = (int*)malloc(n * sizeof(int*));
    memset(bufpos, 0, n * sizeof(int*));

    while (true)
    {
        res = poll(fds, 2 * n, 0);
        if (res > 0)
        {
            for (i = 0; i < n; i++)
            {
                if (fds[2 * i].revents & POLLIN)
                {
                    int freesp = BUFSIZE - bufpos[i];
                    int rd = read(fds[2 * i].fd, bufs[i], freesp);
                    bufpos[i] += rd;
                    if (bufpos[i] == BUFSIZE)
                        fds[2 * i] ^= POLLIN;
                }
                if (fds[2 * i].revents & POLLOUT)
                {
                    
                }
            }
        }
    }

    return 0;
}
