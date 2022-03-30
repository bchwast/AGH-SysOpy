CC = gcc
CFLAGS = -g -Wall

all: rectangles

rectangles:
	$(CC) $(CFLAGS) -c rectangles.c -o rectangles.o
	$(CC) $(CFLAGS) rectangles.o -o rectangles

clean:
	rm *.o rectangles

clean_files:
	rm w*

test:
	rm raport.txt
	$(CC) $(CFLAGS) -c rectangles.c -o rectangles.o -D TEST
	$(CC) $(CFLAGS) rectangles.o -o rectangles
	@./rectangles 0.0001 100
	@./rectangles 0.0001 1000
	@./rectangles 0.0001 10000
	@./rectangles 0.00005 10000
	@./rectangles 0.2 1500
	@./rectangles 0.002 4352
	@./rectangles 0.000001 120
	@./rectangles 0.00001 5400
	@./rectangles 0.0012 3433
	@./rectangles 0.000004 12000
	@./rectangles 0.000003 15000
	@./rectangles 0.000002 13002
