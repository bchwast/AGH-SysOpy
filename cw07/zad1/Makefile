CC = gcc
CFLAGS = -ggdb3 -Wall

all: cook deliverer main

cook:
	$(CC) -o cook -Wall -ggdb3 cook.c

deliverer:
	$(CC) -o deliverer -Wall -ggdb3 deliverer.c

main:
	$(CC) -o main -Wall -ggdb3 main.c

clean:
	rm cook deliverer main