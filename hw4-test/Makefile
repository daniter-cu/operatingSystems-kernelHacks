CC=gcc
CFLAGS=-Wall -I ~/Ker_Hack/linux-2.6.11.12-hmwk4/include
OBJ=main.o test.o test-ras.o util.o

test: $(OBJ)
	$(CC) -o $@ $^ -lpthread

$(OBJ): *.h

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJ) test *~
