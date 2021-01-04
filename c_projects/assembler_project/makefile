assembler: main.o stringOps.o binaryOps.o
		gcc -g -ansi -Wall -pedantic main.o stringOps.o binaryOps.o -o assembler
main.o: main.c main.h utilities.h
		gcc -c -ansi -Wall -pedantic main.c -o main.o
stringOps.o: stringOps.c stringOps.h utilities.h
		gcc -c -ansi -Wall -pedantic stringOps.c -o stringOps.o
binaryOps.o: binaryOps.c binaryOps.h utilities.h
		gcc -c -ansi -Wall -pedantic binaryOps.c -o binaryOps.o