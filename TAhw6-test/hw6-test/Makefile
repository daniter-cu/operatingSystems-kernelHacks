CC=gcc
CFLAGS=-Wall

TESTSRC=test-toctou
CFLAGSTEST=-Wall

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
