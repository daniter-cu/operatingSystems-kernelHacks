CC=gcc
CFLAGS=-g -Wall

CFLAGSTEST=-g -Wall -I ~/Ker_Hack/linux-2.6.11.12-hmwk4/include -static

test: test.o
	$(CC) $(CFLAGSTEST) test.o -o test

test.o: test.c
	$(CC) $(CFLAGSTEST) -c test.c

clean:
	rm -f test.o test
