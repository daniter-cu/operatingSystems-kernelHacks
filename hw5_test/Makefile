#Makefile

CC=gcc
CFLAGS=-Wall
 
 TESTSRC=test-atomic
 CFLAGSTEST=-Wall -I ~/W4118/linux-2.6.11.12-hw3/include -static
  
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

