all : CFLAGS = -Wall -O2
all : main

main : readlines-main

libreadlines.o : 
	gcc ${CFLAGS} -c -fpic readlines.c -o libreadlines.o

libreadlines.so : libreadlines.o
	gcc ${CFLAGS} -shared libreadlines.o -o libreadlines.so

main-simple : 
	gcc ${CFLAGS} -c readlines-main.c

readlines-main : libreadlines.so main-simple
	gcc ${CFLAGS} -s readlines-main.o -Wl,-rpath=. -L. -lreadlines -o readlines-main

clean :
	rm -f readlines-main.o libreadlines.o libreadlines.so
	rm -f readlines-main
