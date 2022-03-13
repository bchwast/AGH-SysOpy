CC = gcc
CFLAGS = -g -Wall

all: static shared

static:
	$(CC) $(CFLAGS) -c library.c -o library.o
	ar rcs liblibrary.a library.o

shared:
	$(CC) $(CFLAGS) -fPIC -c library.c -o library.o
	$(CC) $(CFLAGS) -shared -o liblibrary.so library.o

DLL:
	$(CC) $(CFLAGS) -fPIC -c library.c -o library.o
	$(CC) $(CFLAGS) -shared -o liblibrary.so library.o

clean:
	rm -rf *.o *.a *.so main

main_static: static
	$(CC) $(CFLAGS) -o main main.c library.o

main_shared: shared
	$(CC) $(CFLAGS) -o main -Wl,-rpath=`pwd` main.c library.o

main_dll: DLL
	$(CC) $(CFLAGS) -o main -ldl -D DLL main.c library.o

test:
	printf "main_static:\n" > results3a.txt
	make main_static
	@./main $(shell cat test_files/test.txt) >> results3a.txt
	printf "\n" >> results3a.txt
	make clean

	printf "main_shared:\n" >> results3a.txt
	make main_shared
	@./main $(shell cat test_files/test.txt) >> results3a.txt
	printf "\n" >> results3a.txt
	make clean

	printf "main_dll:\n" >> results3a.txt
	make main_dll
	@./main $(shell cat test_files/test.txt) >> results3a.txt
	make clean