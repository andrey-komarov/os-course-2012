#include <iostream>
#include <map>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>

using namespace std;

char* itoa(int n, char* buf)
{
    int len = 0;
    int nn = n;
    while (nn)
    {
        nn /= 10;
        len++;
    }
    buf[len] = 0;
    while (n)
    {
        buf[--len] = n % 10 + '0';
        n /= 10;
    }
    return buf;
}

struct Event;
map<int, Event*> senders;

#define BUFSIZE (32 * (1 << 20))
struct Event 
{
    enum Type { UNDEF, SENDER, RECEIVER };
    int epollfd;
    int fd;
    Type type;
    char *buf;
    int bufpos;
    int token;
    bool done;
    Event* dual;
    epoll_event ev;
    bool needSend;
   
    char outbuf[20];
    int outbufNeed;
    int outbufWritten;

    Event() {}

    Event(int epollfd, int fd)
        : epollfd(epollfd)
        , fd(fd)
        , type(UNDEF)
        , buf(new char[BUFSIZE])
        , bufpos(0)
    {}

    void runUndef()
    {
        printf("processing undefiend %d\n, pos %d", fd, bufpos);
        int n = read(fd, buf + bufpos, BUFSIZE - bufpos);
        if (n < 0)
        {
            perror("read");
            exit(1);
        }
        bufpos += n;
        if (bufpos >= 5)
        {
            if (strncmp("send\n", buf, 5) == 0)
            {
                type = SENDER;
                memmove(buf, buf + 5, bufpos - 5);
                bufpos -= 5;
                token = rand();
                senders[token] = this;
                done = false;

                itoa(token, outbuf);
                outbufNeed = strlen(outbuf);
                outbufWritten = 0;
                ev.events |= EPOLLOUT;
            }
            else if (strncmp("recv\n", buf, 5) == 0)
            {
                type = RECEIVER;
                memmove(buf, buf + 5, bufpos - 5);
                bufpos -= 5;
                buf[bufpos] = 0;
                done = false;
            }
            else
            {
                printf("FAIL: don't know what to do with <%s>\n", buf);
                exit(1);
            }
        }
    }

    void runSender()
    {
        printf("events = %d IN = %d OUT = %d\n", ev.events, EPOLLIN, EPOLLOUT);
        if (!done)
        {
            int n = read(fd, buf, BUFSIZE - bufpos);
            bufpos += n;
            if (n == 0) 
            {
                done = true;
                ev.events &=~ EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
                printf("done here(%d)\n", fd);
                needSend = true;
            }
        }

        if ((ev.events & EPOLLOUT) && outbufWritten < outbufNeed)
        {
            int n = write(fd, outbuf + outbufWritten, outbufNeed - outbufWritten);
            outbufWritten += n;
            if (outbufWritten == outbufNeed)
            {
                ev.events &=~ EPOLLOUT;
            }
        }
        if (ev.events == 0)
        {
            epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
            close(fd);
        }
    }

    void runReceiver()
    {
        if (!done)
        {
            int n = read(fd, buf, BUFSIZE - bufpos);
            printf("read %d bytes in receiver\n", n);
            printf("buf = %s\n", buf);
            bufpos += n;
            if (n == 0)
            {
                done = true;
                token = atoi(buf);
                printf("received token %d\n", token);
                dual = senders[token];
                dual->dual = this;
                bufpos = 0;
            }
        }
        else
        {
            int n = write(fd, dual->buf + bufpos, dual->bufpos - bufpos);
            if (n == 0)
            {
                printf("%d sending done\n", fd);
            }
            bufpos += n;
            if (bufpos == dual->bufpos && dual->done)
            {
                printf("sending %d to %d done\n", token, fd);
                ev.events = 0;
                epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
                delete[](buf);
                delete[](dual->buf);
                close(fd);
            }
        }
    }

    void run()
    {
        printf("running %d\n", fd);
        if (type == UNDEF)
            runUndef();
        else if (type == SENDER)
            runSender();
        else if (type == RECEIVER)
            runReceiver();
    }
};

int main()
{
    struct addrinfo hints;
    struct addrinfo *servinfo;

    int status;
    memset(&hints, 0, sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;

    if ((status=getaddrinfo(NULL,"3490",&hints,&servinfo)) !=0){
        fprintf(stderr,"getaddrinfo error:%s\n",gai_strerror(
                    status));
        exit(1);
    }
    if (servinfo == NULL)
    {
        printf("servinfo == NULL\n");
        exit(1);
    }
    int sockfd = socket(servinfo->ai_family, 
            servinfo->ai_socktype, servinfo->ai_protocol);
    if (sockfd < 0)
    {
        perror("sockfd < 0");
        exit(1);
    }
    if (bind(sockfd, servinfo->ai_addr, 
                servinfo->ai_addrlen) < 0)
    {
        perror("bind < 0");
        exit(1);
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
        perror("setsockopt < 0");
        exit(1);
    }
    if (listen(sockfd, 5) < 0)
    {
        perror("listen < 0");
        exit(1);
    }
    freeaddrinfo(servinfo);

    int epollfd = epoll_create(1);
    if (epollfd < 0)
    {
        perror("epoll < 0");
        exit(1);
    }

    struct epoll_event ev, events[10];
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) < 0)
    {
        perror("epoll_ctl : add socket");
        exit(1);
    }

    while (1)
    {
        int nfds = epoll_wait(epollfd, events, 10, -1);
        if (nfds < 0)
        {
            perror("epoll_wait");
            exit(1);
        }

        int i;
        for (i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == sockfd)
            {
                int newfd = accept(sockfd, NULL, NULL);
                if (newfd < 0)
                {
                    perror("accept");
                    continue;
                }
                printf("accepted, %d\n", newfd);
                Event* e = new Event(epollfd, newfd);
                e->ev.events = EPOLLIN;
                e->ev.data.ptr = e;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newfd, &e->ev) < 0)
                {
                    perror("epoll_ctl");
                    exit(1);
                }
            }
            else 
            {
                ((Event*)events[i].data.ptr)->run();
            }
        }

    }
}
