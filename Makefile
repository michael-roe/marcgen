
all: marcgen

margcgen: marcgen.o
	gcc -o marcgen marcgen.o

marcgen.o: marcgen.c
	gcc -c marcgen.c
