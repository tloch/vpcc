.PHONY: default all clean test
#default: test
default: all

all: parser

CC=gcc

clean:
	-rm *.o
	-rm parser

test: parser
	./parser < parser.c

parser: scanner.o parser.o testbed.o
	$(CC) -o scanner scanner.o parser.o testbed.o

scanner.o: scanner.c glue.h
	$(CC) -o scanner.o -c scanner.c

parser.o: parser.c glue.h
	$(CC) -o parser.o -c parser.c

glue.o: glue-linux.c glue.h
	$(CC) -o glue.o -c glue-linux.c

testbed.o: testbed.c
	$(CC) -o testbed.o -c testbed.c


