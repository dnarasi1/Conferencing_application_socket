/*--------------------------------------------------------------------*/
/* conference client */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <time.h> 
#include <errno.h>

#include <stdlib.h>

#define MAXMSGLEN  1024

extern char * recvtext(int sd);
extern int sendtext(int sd, char *msg);

extern int hooktoserver(char *servhost, ushort servport);

int fd_isset(int fd, fd_set *fsp) {
      return FD_ISSET(fd, fsp);
  }

/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
	int sock;

    	fd_set rfds;
    	int retval;

	fd_set wfds;

    	/* check usage */
    	if (argc != 3) {
        	fprintf(stderr, "usage : %s <servhost> <servport>\n", argv[0]);
        	exit(1);
   	 }

    	/* get hooke d on to the server */
    	sock = hooktoserver(argv[1], atoi(argv[2]));
    	if (sock == -1) {
        	perror("Error: ");
        	exit(1);
    	}

    	FD_ZERO(&rfds);

    	//FD_SET(0, &rfds);
	//FD_SET(0, &wfds);
    	//FD_SET(sock, &rfds);
	//FD_SET(sock, &wfds);

    	/* keep talking */
    	while (1) {
		FD_SET(0, &rfds);
		FD_SET(0, &wfds);
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(sock, &rfds);
		
		FD_SET(sock, &wfds);
        	retval = select(sock+1, &rfds, &wfds, NULL, NULL);
		if(fd_isset(sock, &rfds)){
            		char *msg;
			msg = (char *)malloc(sizeof(char) *100);
            		msg = recvtext(sock);
            		if (!msg) {
                		/* server killed, exit */
               		 	fprintf(stderr, "error: server died\n");
                		exit(1);
            		}

            		/* display the message */
            		printf(">>> %s", msg);
            		/* free the message */
            		free(msg);
        	}
        	if (fd_isset(sock, &wfds)) {

            		char msg[MAXMSGLEN];
            		if (!fgets(msg, MAXMSGLEN, stdin))
                		exit(0);
            		sendtext(sock, msg);
        	}

        	printf(">");
        	fflush(stdout);
    	}
    	return 0;
}
/*--------------------------------------------------------------------*/

