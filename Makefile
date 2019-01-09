# PROFESIONAL MAKEFILE

Progetto: build/manager.o build/student.o build/utility.o 
	./bin/manager

build/utility.o: src/utility.c src/common.h
	gcc -c src/utility.c -o build/utility.o

build/manager.o: src/manager.c src/common.h build/utility.o
	gcc -Wall src/manager.c build/utility.o -o bin/manager

build/student.o: src/student.c src/common.h build/utility.o
	gcc -Wall src/student.c build/utility.o -o bin/student 

clean:
	rm -f build/* bin/*