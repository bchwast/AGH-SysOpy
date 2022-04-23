CC = gcc
CFLAGS = -ggdb3 -Wall

all: client server

client:
	$(CC) -o client -Wall -ggdb3 client.c

server:
	$(CC) -o server -Wall -ggdb3 server.c

clean:
	rm client server