C = gcc
CFLAGS = -Wall

all: cimin test

cimin.o: cimin.c
	$(CC) $(CFLAGS) -c cimin.c

cimin: cimin.o
	$(CC) $(CFLAGS) -o cimin cimin.o

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

test: test.o
	$(CC) $(CFLAGS) -o test test.o

clean:
	rm -f *.o *.exe cimin test

