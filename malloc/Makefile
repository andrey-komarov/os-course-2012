CC=g++
CFLAGS=-Wall -g -std=c++0x

all : main malloc.so

main : main.o
	${CC} ${CFLAGS} -s main.o -o main

run : main
	LD_PRELOAD=./malloc.so ./main

malloc.o : malloc.h malloc.cpp
	${CC} ${CFLAGS} -shared -fpic -c malloc.cpp -o malloc.o

malloc.so : malloc.o Allocator.h Aligned.h
	${CC} ${CFLAGS} -shared -fpic malloc.o -o malloc.so

main.o : main.c
	gcc -g -c main.c -o main.o

main-debug:
	gcc -g main.c -o main

edit : 
	vim -p *.cpp *.h Makefile
