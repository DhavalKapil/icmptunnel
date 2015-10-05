CC=gcc
CFLAGS=-I.
DEPS = icmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: test_server test_client
	
test_server: icmp_server.o icmp.o test_server.o
	$(CC) -o test_server icmp_server.o icmp.o test_server.o $(CFLAGS)

test_client: icmp_client.o icmp.o test_client.o
	$(CC) -o test_client icmp_client.o icmp.o test_client.o $(CFLAGS)

clean:
	rm -f *.o test_server test_client
