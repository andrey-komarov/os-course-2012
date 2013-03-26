all : 
	gcc -Wall -shared -fpic -o malloc.so malloc.c

test : all
	LD_PRELOAD="`pwd`/malloc.so" ls
