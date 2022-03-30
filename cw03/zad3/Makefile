CC = gcc
CFLAGS = -g -Wall

all: mole

mole:
	$(CC) $(CFLAGS) -c mole.c -o mole.o
	$(CC) $(CFLAGS) mole.o -o mole

clean:
	rm *.o mole