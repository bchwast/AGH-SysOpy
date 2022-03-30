CC = gcc
CFLAGS = -g -Wall

all: catcher sender

catcher:
	$(CC) $(CflagsS) -c catcher.c -o catcher.o
	$(CC) $(CflagsS) catcher.o -o catcher

sender:
	$(CC) $(CflagsS) -c sender.c -o sender.o
	$(CC) $(CflagsS) sender.o -o sender

clean:
	rm *.o catcher sender