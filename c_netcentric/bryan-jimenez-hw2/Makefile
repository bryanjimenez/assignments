include ../Make.defines

PROGS =	sdb-server sdb-client sdb-client_NOGUI

all:	${PROGS}

sdb-server:	sdb-server.o
		${CC} ${CFLAGS} -ansi -pedantic -o $@ sdb-server.o ${LIBS}
		
sdb-client_NOGUI:	sdb-client_NOGUI.o
		${CC} ${CFLAGS} -ansi -pedantic -o $@ sdb-client_NOGUI.o ${LIBS}
		
sdb-client:	sdb-client.c
		gcc -Wall -g -o sdb-client sdb-client.c `pkg-config --cflags --libs gtk+-2.0`
		
clean:
		rm -f ${PROGS} ${CLEANFILES}
