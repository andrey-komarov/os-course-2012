CC=g++
LIBS=-lpthread
CFLAGS=-Wall -std=c++0x -g ${LIBS}

main : main.o malloc.so
	${CC} ${CFLAGS} -s main.o malloc.o -o main

run : main
	LD_PRELOAD=malloc.so vim

malloc.o : malloc.h malloc.cpp
	${CC} ${CFLAGS} -c malloc.cpp -o malloc.o

malloc.so : malloc.o
	${CC} ${CFLAGS} -shared -fpic -o malloc.so

main.o : malloc.o main.cpp
	${CC} ${CFLAGS} -c main.cpp -o main.o

