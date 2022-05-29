CC = gcc
CFLAGS = -ggdb3 -Wall

all: server client

server:
	$(CC) -o server -Wall -ggdb3 server.c -lpthread

client:
	$(CC) -o client -Wall -ggdb3 client.c -lpthread

clean:
	rm server client socket