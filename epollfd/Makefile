CC=g++
CFLAGS=-Wall -g -std=c++0x

all : Buffer.o EpollFD.o ASyncOperation.o

Buffer.o : Buffer.cpp
	${CC} ${CFLAGS} -c -o Buffer.o Buffer.cpp

EpollFD.o : EpollFD.cpp
	${CC} ${CFLAGS} -c -o EpollFD.o EpollFD.cpp

ASyncOperation.o : ASyncOperation.cpp
	${CC} ${CFLAGS} -c -o ASyncOperation.o ASyncOperation.cpp
