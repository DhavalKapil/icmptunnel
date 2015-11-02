CC=gcc
CFLAGS=-I.
DEPS = icmp.h tunnel.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

icmptunnel: icmptunnel.o icmp.o tunnel.o
	$(CC) -o icmptunnel icmp.o tunnel.o icmptunnel.o $(CFLAGS)

test_server: icmp.o test_server.o
	$(CC) -o test_server icmp.o test_server.o $(CFLAGS)

test_client: icmp.o test_client.o
	$(CC) -o test_client icmp.o test_client.o $(CFLAGS)

test: test_server test_client

all: icmptunnel test_server test_client

clean:
	rm -f *.o test_server test_client icmptunnel
