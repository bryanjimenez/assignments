include ../Make.defines

PROGS =	myserver

all:	${PROGS}

myserver:	myserver.o
		${CC} ${CFLAGS} -ansi -pedantic -o $@ myserver.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
