CC = gcc
CFLAGS = -g -Wall

all: flags

flags:
	$(CC) $(CflagsS) -c flags.c -o flags.o
	$(CC) $(CflagsS) flags.o -o flags

clean:
	rm *.o flags