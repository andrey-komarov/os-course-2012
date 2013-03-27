all : 
	gcc -Wall -g -shared -fpic -o malloc.so malloc.c

test :
	gcc -g -Wall -o malloc malloc.c
	./malloc
