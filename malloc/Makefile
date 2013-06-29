CC=g++
LIBS=-lpthread
CFLAGS=-Wall -std=c++0x -O2 ${LIBS}

main : main.o malloc.so
	${CC} ${CFLAGS} -lstdc++ -s main.o -o main

run : main
	LD_PRELOAD=./malloc.so ./main

malloc.o : malloc.h malloc.cpp
	${CC} ${CFLAGS} -c malloc.cpp -o malloc.o

malloc.so : malloc.o Allocator.h Aligned.h
	${CC} ${CFLAGS} -shared -fpic -o malloc.so

main.o : malloc.o main.cpp
	${CC} ${CFLAGS} -c main.cpp -o main.o

edit : 
	vim -p *.cpp *.h Makefile
