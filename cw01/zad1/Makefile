CC = gcc
CFLAGS = -g -Wall

all: static shared

static:
	$(CC) $(CFLAGS) -c library.c -o library.o
	ar rcs liblibrary.a library.o

shared:
	$(CC) $(CFLAGS) -fPIC -c library.c -o library.o
	$(CC) $(CFLAGS) -shared -o liblibrary.so library.o

clean:
	rm -rf *.o *.a *.so main
