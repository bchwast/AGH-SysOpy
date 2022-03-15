CC = gcc
CFLAGS = -g -Wall

all: lib sys

lib:
	$(CC) $(CFLAGS) -c whitespace_killer_lib.c -o lib.o
	$(CC) $(CFLAGS) lib.o -o lib

sys:
	$(CC) $(CFLAGS) -c whitespace_killer_sys.c -o sys.o
	$(CC) $(CFLAGS) sys.o -o sys

clean:
	rm *.o lib sys