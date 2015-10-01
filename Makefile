CC=gcc
CFLAGS=-I.
DEPS = icmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: icmp.o test.o
	$(CC) -o test icmp.o test.o $(CFLAGS)

clean:
	rm -f *.o test
