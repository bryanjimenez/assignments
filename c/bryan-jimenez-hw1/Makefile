all: replace

replace: replace.o
	gcc replace.o  -o replace

replace.o: replace.c
	gcc -c replace.c -o replace.o

clean:
	rm -f replace.o replace *~ core
