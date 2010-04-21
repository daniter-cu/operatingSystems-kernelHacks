#Makefile

CC=gcc
CFLAGS=-Wall
 
 TESTSRC=hw6test
 CFLAGSTEST=-Wall -I ~/linux-2.6.11.12-hmwk5/include -static
  
test: main.o test.o $(TESTSRC).o
	$(CC) -o $@ $^ -lpthread
	 
main.o: main.c
	$(CC) $(CFLAGS) -c $^
	 
test.o: test.c
	$(CC) $(CFLAGS) -c $^
	 
$(TESTSRC).o: $(TESTSRC).c *.h
	$(CC) $(CFLAGSTEST) -c $<
	 
clean:
	rm -f main.o test.o $(TESTSRC).o test

