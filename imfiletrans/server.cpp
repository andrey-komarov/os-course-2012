#include <iostream>

#include<sys/types.h>
#include<sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>

using namespace std;

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
                int pid = fork();
                if (pid == 0)
                {
                    close(sockfd);
                    dup2(newfd, 0);
                    dup2(newfd, 1);
                    dup2(newfd, 2);
                    close(newfd);
                    execlp("echo", "echo", "hello", NULL);
                    exit(0);
                }
                close(newfd);
            }
        }

    }
}
