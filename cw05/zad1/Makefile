CC = gcc
CFLAGS = -g -Wall

all: interpreter 

interpreter:
	$(CC) $(CflagsS) -c interpreter.c -o interpreter.o
	$(CC) $(CflagsS) interpreter.o -o interpreter

clean:
	rm *.o interpreter