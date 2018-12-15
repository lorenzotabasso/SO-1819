# Compiler's Flags
CFLAGS = -g -Wall -Wpedantic

build/student.o: src/student.c src/student.h Makefile
	$(CC) $(CFLAGS) -c src/student.c -o build/student.o

build/gestore.o: src/gestore.c src/gestore.h Makefile
	$(CC) $(CFLAGS) -c src/gestore.c -o build/gestore.o

bin/gestore: build/student.o build/gestore.o
	$(CC) -o bin/gestore build/student.o build/gestore.o

all: bin/gestore

clean:
	rm -f build/* bin/*
