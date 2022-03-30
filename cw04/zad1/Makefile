CC = gcc
CFLAGS = -g -Wall

all: fork exec

fork:
	$(CC) $(CFLAGS) -c raiser.c -o fork.o -D FORK
	$(CC) $(CFLAGS) fork.o -o fork

exec: child
	$(CC) $(CFLAGS) -c raiser.c -o exec.o -D EXEC
	$(CC) $(CFLAGS) exec.o -o exec

child: 
	$(CC) $(CFLAGS) -c child.c -o child.o
	$(CC) $(CFLAGS) child.o -o child

clean:
	rm *.o fork exec child