#Makefile

CC=gcc
CFLAGS=-Wall
 
 TESTSRC=hw6test
 CFLAGSTEST=-Wall -I ~/Ker_hack/linux-2.6.11.12-hmwk6/include -static
  
test: main.o test.o $(TESTSRC).o
	sudo useradd alice
	touch secretfile
	sudo chown alice:alice secretfile
	$(CC) -o $@ $^ -lpthread
	chmod u+s test
	sudo chown alice:alice test
	 
main.o: main.c
	$(CC) $(CFLAGS) -c $^
	 
test.o: test.c
	$(CC) $(CFLAGS) -c $^
	 
$(TESTSRC).o: $(TESTSRC).c *.h
	$(CC) $(CFLAGSTEST) -c $<
	 
clean:
	rm -f main.o test.o $(TESTSRC).o test
	sudo userdel alice
	sudo rm secretfile

