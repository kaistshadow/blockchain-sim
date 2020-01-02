CC = g++
CC_OPTS = -Wall -Werror -lev

all: array-heap.o unix-echo-server unix-echo-client udp-echo-server test-server test-client

clean:
	rm -f *.o unix-echo-server unix-echo-client udp-echo-server

array-test: array-test.c array-heap.o
	$(CC) $(CC_OPTS) -o $@ $< array-heap.o

array-heap.o: array-heap.c array-heap.h
	$(CC) $(CC_OPTS) -o $@ -c $<

udp-echo-server: udp-echo-server.c
	$(CC) $(CC_OPTS) -o $@ $< -lev

unix-echo-server: unix-echo-server.c array-heap.o
	$(CC) $(CC_OPTS) -o $@ $< array-heap.o -lev

unix-echo-client: unix-echo-client.c
	$(CC) $(CC_OPTS) -o $@ $< -lev

test-server: test-server.c array-heap.o
	$(CC) $(CC_OPTS) -o $@ $< array-heap.o -lev

test-client: test-client.c
	$(CC) $(CC_OPTS) -o $@ $< -lev
