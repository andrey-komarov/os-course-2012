CC=g++
CFLAGS=-Wall -g -std=c++0x

all : Buffer.o EpollFD.o ASyncOperation.o main.o main

debug :
	${CC} ${CFLAGS} *.cpp -o main
	gdb -tui main

Buffer.o : Buffer.cpp
	${CC} ${CFLAGS} -c -o Buffer.o Buffer.cpp

EpollFD.o : EpollFD.cpp
	${CC} ${CFLAGS} -c -o EpollFD.o EpollFD.cpp

ASyncOperation.o : ASyncOperation.cpp
	${CC} ${CFLAGS} -c -o ASyncOperation.o ASyncOperation.cpp

main.o : main.cpp
	${CC} ${CFLAGS} -c -o main.o main.cpp

main : main.o ASyncOperation.o EpollFD.o Buffer.o
	${CC} ${CFLAGS} -s main.o ASyncOperation.o EpollFD.o Buffer.o -o main
