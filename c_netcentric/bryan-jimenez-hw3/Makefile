all:	sender twister receiver

sender:	sender.c
	gcc -ansi -pedantic sender.c -o sender -pthread
	
twister:	twister.c
	gcc -ansi -pedantic twister.c -o twister -pthread
	
receiver:	receiver.c
	gcc -ansi -pedantic receiver.c -o receiver	

clean:
	rm -f *.o core *~ sender twister receiver
