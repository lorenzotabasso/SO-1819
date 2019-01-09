Progetto: manager.o student.o utility.o 
	./manager

manager.o: manager.c  common.h utility.o
	gcc -Wall manager.c -o manager utility.o

student.o: student.c  common.h utility.o
	gcc -Wall student.c -o student utility.o

utility.o: utility.c
	gcc -c utility.c


