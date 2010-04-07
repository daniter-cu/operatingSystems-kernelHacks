CC=gcc
CFLAGS=-g -Wall

CFLAGSTEST=-g -Wall -I ./linux-2.6.11.12-hmwk4/include -static

team17test: team17test.o
	$(CC) $(CFLAGSTEST) team17test.o -o team17test

team17test.o: team17test.c
	$(CC) $(CFLAGSTEST) -c team17test.c

clean:
	rm -f team17test.o team17test
