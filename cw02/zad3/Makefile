CC = gcc
CFLAGS = -g -Wall

all: stat nftw

stat:
	$(CC) $(CFLAGS) -c directory_stat.c -o stat.o
	$(CC) $(CFLAGS) stat.o -o stat

nftw:
	$(CC) $(CFLAGS) -c directory_nftw.c -o nftw.o
	$(CC) $(CFLAGS) nftw.o -o nftw

clean_stat:
	rm *.o stat

clean_nftw:
	rm *.o nftw

clean: clean_stat clean_nftw