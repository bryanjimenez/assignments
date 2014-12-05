all: genmat prtmat mpimm

genmat: genmat.o
	gcc genmat.o  -o genmat
	
genmat.o: genmat.c
	gcc -c genmat.c -o genmat.o
	
prtmat: prtmat.o
	gcc prtmat.o  -o prtmat
	
prtmat.o: prtmat.c
	gcc -c prtmat.c -o prtmat.o	
	
mpimm: mpimm.c
	mpicc -ansi -pedantic mpimm.c -o mpimm	

clean:
	$(RM) genmat prtmat mpimm
	$(RM) *.o core *~
	$(RM) -r *.dSYM
