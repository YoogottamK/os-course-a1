CFLAGS = -g -Wall

all: part1

part1: utils.o part1.o file.o
	gcc $(CFLAGS) -o part1 utils.o part1.o file.o

part1.o: part1.c
	gcc $(CFLAGS) -c part1.c

file.o: file.h file.c utils.o
	gcc $(CFLAGS) -c file.c

utils.o: utils.h utils.c
	gcc $(CFLAGS) -c utils.c

clean:
	rm *.o part1
