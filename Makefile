.c.o:
	gcc -g -c $?

# compile client and server
all: confclient confserver

# compile client only
confclient: confclient.o confutils.o
	gcc -g -o confclient confclient.o  confutils.o

# compile server program
confserver: confserver.o confutils.o
	gcc -g -o confserver confserver.o  confutils.o

clean:
	rm -f *.o confclient confserver
