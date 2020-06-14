/*--------------------------------------------------------------------*/
/* functions to connect clients and server */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <time.h> 
#include <errno.h>

#include <stdlib.h>

#define MAXNAMELEN 256
/*--------------------------------------------------------------------*/

/*----------------------------------------------------------------*/
/* prepare server to accept requests
 returns file descriptor of socket
 returns -1 on error
 */
int startserver() {
    int sd; /* socket descriptor */
    struct sockaddr_in server_addr, server_p;
    
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    
    char * servhost; /* full name of this host */
    servhost = (char*)malloc(sizeof(char)*50);
    ushort servport; /* port assigned to this server */
    struct hostent * host_name; /*used by gethostbyname() */

   
    sd = socket(PF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(0);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sd, (struct sockaddr *)&server_addr,  
                                 sizeof(server_addr));
    /* we are ready to receive connections */
    int j=  listen(sd, 5);
    gethostname(servhost, sizeof(servhost));
    host_name = gethostbyname(servhost);
    
    //returns struct hostent    
    strcpy(servhost, host_name->h_name);

     int len = sizeof(server_p);
     getsockname(sd, (struct sockaddr *)&server_p, &len);
     servport = ntohs(server_p.sin_port);

    /* ready to accept requests */
    printf("admin: started server on '%s' at '%hu'\n", servhost, servport);
    free(servhost);
    return(sd);
}
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------*/
/*
 establishes connection with the server
 returns file descriptor of socket
 returns -1 on error
 */
int hooktoserver(char *servhost, ushort servport) {
    int sd; /* socket descriptor */
    struct sockaddr_in address, client_p;
    ushort clientport; /* port assigned to this client */

    struct hostent * host_name;    

    /*
     FILL HERE
     create a TCP socket using socket()
     */
    sd = socket(PF_INET, SOCK_STREAM, 0);
    
    /*
     FILL HERE
     connect to the server on 'servhost' at 'servport'
     use gethostbyname() and connect()
     */
     host_name = gethostbyname(servhost);
     
     address.sin_family = AF_INET;
     address.sin_port = htons(servport);
     address.sin_addr = *((struct in_addr *)host_name->h_addr);

     int q = connect(sd, (struct sockaddr *)&address, sizeof(address));
    /*
     FILL HERE
     figure out the port assigned to this client
     use getsockname()
     */
     int len = sizeof(client_p);
     getsockname(sd, (struct sockaddr *)&client_p, &len);
     clientport = ntohs(client_p.sin_port);


    /* succesful. return socket descriptor */
    printf("admin: connected to server on '%s' at '%hu' thru '%hu'\n", servhost,
            servport, clientport);
    printf(">");
    fflush(stdout);
    return (sd);
}
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------*/
int readn(int sd, char *buf, int n) {
    int toberead;
    char * ptr;
    toberead = n;
    ptr = buf;
    while (toberead > 0) {
        int byteread;
        byteread = read(sd, ptr, toberead);
        if (byteread <= 0) {
            if (byteread == -1)
                perror("read");
            return (0);
        }

        toberead -= byteread;
        ptr += byteread;
    }
    return (1);
}

char *recvtext(int sd) {
    char *msg;
    long len;

    /* read the message length */
    if (!readn(sd, (char *) &len, sizeof(len))) {
        return (NULL);
    }
    len = ntohl(len);
    /* allocate space for message text */
    msg = NULL;
    if (len > 0) {
        msg = (char *) malloc(sizeof(char)*len);
        if (!msg) {
            fprintf(stderr, "error : unable to malloc\n");
            return (NULL);
        }

        /* read the message text */
        if (!readn(sd, msg, len)) {
            free(msg);
            return (NULL);
        }
    }

    /* done reading */
    return (msg);
}

int sendtext(int sd, char *msg) {
    long len;

    /* write lent */
    len = (msg ? strlen(msg) + 1 : 0);
    len = htonl(len);
    write(sd, (char *) &len, sizeof(len));

    /* write message text */
    len = ntohl(len);
    if (len > 0)
        write(sd, msg, len);
    return (1);
}
/*----------------------------------------------------------------*/

