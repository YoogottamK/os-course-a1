CFLAGS = -g -Wall

all: part1 part2

part1: utils.o file.o part1.o
	gcc $(CFLAGS) -o part1 utils.o file.o part1.o

part2: utils.o file.o part2.o
	gcc $(CFLAGS) -o part2 utils.o file.o part2.o

part1.o: part1.c
	gcc $(CFLAGS) -c part1.c

part2.o: part2.c
	gcc $(CFLAGS) -c part2.c

file.o: file.h file.c utils.o
	gcc $(CFLAGS) -c file.c

utils.o: utils.h utils.c
	gcc $(CFLAGS) -c utils.c

clean:
	rm *.o part1 part2
