CC=gcc
CFLAGS=-I.
DEPS = icmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

icmp_tunnel: icmp_tunnel.o icmp.o tunnel.o
	$(CC) -o icmp_tunnel icmp.o tunnel.o icmp_tunnel.o $(CFLAGS)

all: icmp_tunnel test_server test_client

test_server: icmp.o test_server.o
	$(CC) -o test_server icmp.o test_server.o $(CFLAGS)

test_client: icmp.o test_client.o
	$(CC) -o test_client icmp.o test_client.o $(CFLAGS)

clean:
	rm -f *.o test_server test_client icmp_tunnel
