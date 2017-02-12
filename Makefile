CC=gcc
CFLAGS=-Wall

scanner: scanner.o semantic.o parser.o 
	$(CC) -o scanner scanner.o semantic.o parser.o 
