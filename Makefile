CC=gcc
CFLAGS=-Wall

all: solver

solver: solver.o
	$(CC) $(CFLAGS) solver.o -o solver

solver.o: solver.c
	$(CC) $(CFLAGS) -c solver.c

clean:
	rm -rf *.o solver a.out
