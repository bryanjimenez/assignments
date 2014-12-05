CFLAGS = -Wall -g #-O2 # when debug change it to -Wall -g -O0

all: genmat prtmat threadmm

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

genmat: genmat.c
	$(CC) $(CFLAGS) -o $@ genmat.c

prtmat: prtmat.c
	$(CC) $(CFLAGS) -o $@ prtmat.c

threadmm: threadmm.c
	$(CC) $(CFLAGS) -pthread -o $@ threadmm.c -lm

clean:
	$(RM) genmat prtmat threadmm
	$(RM) *.o core *~
	$(RM) -r *.dSYM
