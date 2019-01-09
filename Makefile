# Compiler's Flags
CFLAGS = -g -Wall -Wpedantic

build/student.o: src/student.c src/utility.c src/common.h Makefile
	$(CC) $(CFLAGS) -c src/student.c -o build/student.o

build/manager.o: src/manager.c src/utility.c src/common.h Makefile
	$(CC) $(CFLAGS) -c src/manager.c -o build/manager.o

build/utility.o: src/utility.c src/common.h Makefile
	$(CC) $(CFLAGS) -c src/utility.c -o build/utility.o

#bin/manager: build/student.o src/utility.o build/manager.o build/utility.o
#	$(CC) -o bin/manager build/student.o build/manager.o build/utility.o

bin/manager: build/manager.o build/utility.o
	$(CC) -o bin/manager build/manager.o build/utility.o

bin/student: build/student.o build/utility.o bin/student

all: bin/manager
	./bin/manager

clean:
	rm -f build/* bin/*