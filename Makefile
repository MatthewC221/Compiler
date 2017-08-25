CC=gcc
CFLAGS=-Wall -g

scanner: scanner.o semantic.o parser.o 
	$(CC) -o scanner scanner.o semantic.o parser.o 
