CC=gcc
CFLAGS=-I.
DEPS = icmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: icmp_server.o icmp_client.o icmp.o test.o
	$(CC) -o test icmp_server.o icmp_client.o icmp.o test.o $(CFLAGS)

clean:
	rm -f *.o test
