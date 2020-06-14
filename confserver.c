/*--------------------------------------------------------------------*/
/* conference server */

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

extern char * recvtext(int sd);
extern int sendtext(int sd, char *msg);

extern int startserver();
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
int fd_isset(int fd, fd_set *fsp) {
    return FD_ISSET(fd, fsp);
}
/* main routine */
int main(int argc, char *argv[]) {
	int servsock; /* server socket descriptor */
	int csd;
	int f;
    	struct timeval tv;
	struct hostent * host_name, *client_host;
	int size;
	char * ptr;
	struct in_addr hipaddr;

	char * msg;
	msg = (char*)malloc(sizeof(char)*200);
	
    	fd_set livesdset; /* set of live client sockets */
	fd_set rdset;
    	int livesdmax; /* largest live client socket descriptor */

	struct sockaddr_in peer, address, client;
	int peer_len;

    	/* check usage */
    	if (argc != 1) {
        	fprintf(stderr, "usage : %s\n", argv[0]);
        	exit(1);
    	}

    	/* get ready to receive requests */
    	servsock = startserver();
    	if (servsock == -1) {
        	perror("Error on starting server: ");
        	exit(1);
    	}

    	/*
     	FILL HERE:
     	init the set of live clients
     	*/
        
   	FD_ZERO(&livesdset);
	FD_SET(servsock,&livesdset);
	livesdmax = servsock;
	

    	/* receive requests and process them */
      	while (1) {
		rdset = livesdset;
           	int frsock; /* loop variable */
        	/*
         	FILL HERE
         	wait using select() for
         	messages from existing clients and
         	connect requests from new clients
         	*/
		

	
         	f = select(livesdmax+1, &rdset, NULL,  NULL, NULL);
 		
		/* look for messages from live clients */
        	for (frsock = 3; frsock <= livesdmax; frsock++) {
            		/* skip the listen socket */
            		/* this case is covered separately */
				
            		if (frsock == servsock)
                		continue;


            		if (fd_isset(frsock, &rdset)) {
            			char * clienthost; /* host name of the client */
                		ushort clientport; /* port number of the client */
				
				clienthost = (char *)malloc(sizeof(char)*100);	
				peer_len = sizeof(peer);
				getpeername(frsock, (struct sockaddr*)&peer, &peer_len);	
				clientport = ntohs(peer.sin_port);
				

				struct in_addr ipAddr1 = peer.sin_addr;
				char str1[INET_ADDRSTRLEN];
				inet_ntop( AF_INET, &ipAddr1, str1, INET_ADDRSTRLEN );

				host_name = gethostbyaddr(&ipAddr1, sizeof(ipAddr1), AF_INET);
			        strcpy(clienthost, host_name->h_name);
						
                		/*
                 		FILL HERE:
                 		figure out client's host name and port
                 		using getpeername() and gethostbyaddr()
                 		*/
				
                		/* read the message */
               			msg = recvtext(frsock);
				

                		if (!msg) {
                			/* disconnect from client */
                    			printf("admin: disconnect from '%s(%hu)'\n", clienthost,
                            		clientport);

                    			/*
                     			FILL HERE:
                     			remove this guy from the set of live clients
                     			*/
					FD_CLR(frsock, &livesdset);

                    			/* close the socket */
                  			close(frsock);
             			} else {
                    			/*
                     			FILL HERE
                     			send the message to all live clients
                     			except the one that sent the message
                     			*/
					for (int fr = servsock; fr <= livesdmax; fr++){
						if(fr!=frsock && fr!=servsock)	
						    sendtext(fr, msg);
					}

                        		/* display the message */
                    			printf("%s(%hu): %s", clienthost, clientport, msg);

                    			/* free the message */
                    			free(msg);
               			}
				free(clienthost);
            		}
        	}
        	/* look for connect requests */
		
        	if (fd_isset(servsock, &rdset)) {
        		/*
             		FILL HERE:
             		accept a new connection request
             		*/
			size = sizeof (address);
			csd = accept(servsock, (struct sockaddr *)&address,(socklen_t*)&size);

			if(csd>livesdmax)
				livesdmax = csd;
			
            		
			/* if accept is fine? */
            		if (csd != -1) {
                		char * clienthost; /* host name of the client */
                		ushort clientport; /* port number of the client */
				
				clienthost = (char*)malloc(sizeof(char)*50);
                		/*
                 		FILL HERE:
                 		figure out client's host name and port
                 		using gethostbyaddr() and without using getpeername().
                 		*/
				
				struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&address;
				struct in_addr ipAddr = pV4Addr->sin_addr;
				char str[INET_ADDRSTRLEN];
				inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
				client_host = gethostbyaddr(&ipAddr, sizeof(ipAddr), AF_INET);


				strcpy(clienthost, client_host->h_name);

	
				clientport = ntohs(address.sin_port);
                		printf("admin: connect from '%s' at '%hu'\n", clienthost,
                        	clientport);
				

                		/*
                 		FILL HERE:
                 		add this guy to set of live clients
                 		*/
				FD_SET(csd,&livesdset);
				free(clienthost);
            		} 
			else {
                		perror("accept");
                 		exit(0);
				
            		}
        	}
    	}
    	return 0;
}
/*--------------------------------------------------------------------*/

