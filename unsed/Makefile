all : CFLAGS = -Wall -O2
all : main

main : readlines-main

readlines.o :
	gcc ${CFLAGS} -c readlines.c

libreadlines.o : 
	gcc ${CFLAGS} -c -fPIC readlines.c -o libreadlines.o

libreadlines.so : libreadlines.o
	gcc ${CFLAGS} -shared libreadlines.o -o libreadlines.so

main-simple : 
	gcc ${CFLAGS} -c readlines-main.c

readlines-main : libreadlines.so main-simple
	gcc ${CFLAGS} -s readlines-main.o -Wl,-rpath=. -L. -lreadlines -o readlines-main
