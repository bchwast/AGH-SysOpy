CC = gcc
CFLAGS = -g -Wall

all: child

child:
	$(CC) $(CFLAGS) -c child_maker.c -o child_maker.o
	$(CC) $(CFLAGS) child_maker.o -o child_maker

clean:
	rm *.o child_maker