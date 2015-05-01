.PHONY: default all clean test
default: test
#default: all

all: scanner

CC=gcc

clean:
	-rm *.o
	-rm scanner

test: scanner
	./scanner < scanner.c

scanner: scanner.o testbed.o
	$(CC) -o scanner scanner.o testbed.o

scanner.o: scanner.c
	$(CC) -o scanner.o -c scanner.c

testbed.o: testbed.c
	$(CC) -o testbed.o -c testbed.c


