CC = gcc
CFLAGS = -g -Wall

all: consumer producer tests

consumer:
	$(CC) $(CflagsS) -c consumer.c -o consumer.o
	$(CC) $(CflagsS) consumer.o -o consumer

producer:
	$(CC) $(CflagsS) -c producer.c -o producer.o
	$(CC) $(CflagsS) producer.o -o producer

tests:
	$(CC) $(CflagsS) -c tests.c -o tests.o
	$(CC) $(CflagsS) tests.o -o tests

test:
	./tests

clean:
	rm *.o consumer producer tests