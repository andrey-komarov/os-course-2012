CC=gcc
CFLAGS=-Wall -g

all : cp-poll cp-epoll

cp-poll : cp-poll.c
	${CC} ${CFLAGS} cp-poll.c -o cp-poll

cp-epoll : cp-epoll.c
	${CC} ${CFLAGS} cp-epoll.c -o cp-epoll

run : cp-epoll
	./cp-epoll 0 1
