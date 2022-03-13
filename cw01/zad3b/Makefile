CC = gcc
CFLAGS = -g -Wall
OT = O1

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
	$(CC) $(CFLAGS) -o main main.c -$(OT) library.o

main_shared: shared
	$(CC) $(CFLAGS) -o main -Wl,-rpath=`pwd` main.c -$(OT) library.o

main_dll: DLL
	$(CC) $(CFLAGS) -o main -ldl -D DLL main.c -$(OT) library.o

test:
	printf "main_static:\n" > results3b.txt
	printf "O0:\n" >> results3b.txt
	make main_static OT=O0
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "O2:\n" >> results3b.txt
	make main_static OT=O2
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "O3:\n" >> results3b.txt
	make main_static OT=O3
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "Os:\n" >> results3b.txt
	make main_static OT=Os
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "main_shared:\n" >> results3b.txt
	printf "O0:\n" >> results3b.txt
	make main_shared OT=O0
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "O2:\n" >> results3b.txt
	make main_shared OT=O2
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "O3:\n" >> results3b.txt
	make main_shared OT=O3
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "Os:\n" >> results3b.txt
	make main_shared OT=Os
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "main_dll:\n" >> results3b.txt
	printf "O0:\n" >> results3b.txt
	make main_dll OT=O0
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "O2:\n" >> results3b.txt
	make main_dll OT=O2
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "O3:\n" >> results3b.txt
	make main_dll OT=O3
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	printf "\n" >> results3b.txt
	make clean

	printf "Os:\n" >> results3b.txt
	make main_dll OT=Os
	@./main $(shell cat test_files/test.txt) >> results3b.txt
	make clean