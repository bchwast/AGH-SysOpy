CC = gcc
CFLAGS = -ggdb3 -Wall

all: negate

negate:
	$(CC) -o negate -Wall -ggdb3 negate.c -lpthread -lm

test: negate
		@./negate 1 numbers images/baboon.ascii.pgm negatives/baboon.pgm > times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 2 numbers images/baboon.ascii.pgm negatives/baboon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 4 numbers images/baboon.ascii.pgm negatives/baboon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 8 numbers images/baboon.ascii.pgm negatives/baboon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 16 numbers images/baboon.ascii.pgm negatives/baboon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt

		@./negate 1 block images/baboon.ascii.pgm negatives/baboon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 2 block images/baboon.ascii.pgm negatives/baboon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 4 block images/baboon.ascii.pgm negatives/baboon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 8 block images/baboon.ascii.pgm negatives/baboon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 16 block images/baboon.ascii.pgm negatives/baboon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt

		@./negate 1 numbers images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 2 numbers images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 4 numbers images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 8 numbers images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 16 numbers images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt

		@./negate 1 block images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 2 block images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 4 block images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 8 block images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 16 block images/dragon.ascii.pgm negatives/dragon.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt

		@./negate 1 numbers images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 2 numbers images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 4 numbers images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 8 numbers images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 16 numbers images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt

		@./negate 1 block images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 2 block images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 4 block images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 8 block images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
		@echo "\n====================================\n" >> times.txt
		@./negate 16 block images/fractal_tree.ascii.pgm negatives/fractal_tree.pgm >> times.txt
clean:
	rm negate