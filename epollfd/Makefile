CC=g++
CFLAGS=-Wall -g -std=c++0x

all : Buffer.o EpollFD.o ASyncOperation.o main.o main

debug :
	${CC} ${CFLAGS} *.cpp -o main
	gdb -tui main

Buffer.o : Buffer.cpp Buffer.h
	${CC} ${CFLAGS} -c -o Buffer.o Buffer.cpp

EpollFD.o : EpollFD.cpp EpollFD.h
	${CC} ${CFLAGS} -c -o EpollFD.o EpollFD.cpp

ASyncOperation.o : ASyncOperation.cpp EpollFD.h
	${CC} ${CFLAGS} -c -o ASyncOperation.o ASyncOperation.cpp

main.o : main.cpp EpollFD.o
	${CC} ${CFLAGS} -c -o main.o main.cpp

main : main.o ASyncOperation.o EpollFD.o Buffer.o
	${CC} ${CFLAGS} -s main.o ASyncOperation.o EpollFD.o Buffer.o -o main

edit:
	vim -p EpollFD.h EpollFD.cpp ASyncOperation.cpp main.cpp
