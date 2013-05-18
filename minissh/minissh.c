#include<sys/types.h>
#include<sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    struct addrinfo hints;
    struct addrinfo *servinfo;
    int status;
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_UNSPEC; //don’tcareIPv4orIPv6
    hints.ai_socktype=SOCK_STREAM;//TCPstreamsockets
    hints.ai_flags=AI_PASSIVE; //fillinmyIPforme
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
    int fd = -1;
    while (1)
    {
        fd = accept(sockfd, NULL, NULL);
        printf("accepted, %d\n", fd);
        if (fd < 0)
        {
            perror("fd < 0");
            continue;
        }
        int pid = fork();
        if (pid == 0)
        {
            close(sockfd);
            dup2(fd, 0);
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);
            //execvp("cat", "cat", "-", NULL);
            execlp("echo", "echo", "hello", NULL);
            exit(0);
        }
        close(fd);
    }
}







