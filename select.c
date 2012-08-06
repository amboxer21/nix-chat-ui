#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
/* master file descriptor list */
fd_set master;
/* temp file descriptor list for select() */
fd_set read_fds;

char buffer[4096];
struct sockaddr_in serv_addr, cli_addr;
struct hostent *server;
/* maximum file descriptor number */
int sockfd, newsockfd, fdmax, binder, listener;
int sockopt, portno, selector, i, j, yes = 1;
int bytes_read, bytes_written;
socklen_t clilen; //addrlen
/* clear the master and temp sets */
FD_ZERO(&master);
FD_ZERO(&read_fds);

	if(argc != 3) {
	printf("USAGE: %s + IP Address + Port No.\n", argv[0]);
	exit(EXIT_FAILURE);
	}

/* get the listener */
sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
	printf("SOCKET(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}
		else if(sockfd) {
		printf("Successfully created socket.\n");
		}

sockopt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if(sockopt == -1) {
	printf("SETSOCKOPT(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}
		else {
		printf("Setsockopt() was successful.\n");
		}

server = gethostbyname(argv[1]); 
	if(server == NULL) {
	printf("GETHOSTBYNAME(NULL) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}

portno = atoi(argv[2]);
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
serv_addr.sin_port = htons(portno);
memset(&(serv_addr.sin_zero), '\0', 8);

binder = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(binder < 0) {
	printf("BIND(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}
		else {
		printf("Bind was successful.\n");
		}

listener = listen(sockfd, 10);
	if(listener == -1) {
	printf("Listen(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}
		else {
		printf("Listening for incoming connections.\n");
		}

//Add the curent FD to the masterset.
FD_SET(sockfd, &master);
//Watch the current FD.
fdmax = sockfd; 

/* Main loop */
printf("Main loop.\n");

for( ; ; ) {
/* copy it */
read_fds = master;

//fdmax inside of select needs +1 in order to work.
selector = select(fdmax+1, &read_fds, NULL, NULL, NULL);
	if(selector == -1) {
	printf("SELECT(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}
		else {
		printf("\n\nSelect() was successful.\n");
		}

/*run through the existing connections looking for data to be read*/
for(i = 0; i <= fdmax; i++) {
	if(FD_ISSET(i, &read_fds)) { 

	if(i == sockfd) {	
	printf("i == sockfd DEBUG MSG --> %s.\n", strerror(errno));

	clilen = sizeof(cli_addr);

	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	if(newsockfd == -1) {
	printf("Accept(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	} 

		else {
		printf("Accept() was successful.\n");
		printf("NEWSOCKFD(1) DEBUG MSG --> %s.\n", strerror(errno));

		 //Add new FD to masterset.
		 FD_SET(newsockfd, &master); 
		 printf("newsockfd == %d\n", newsockfd);
		 printf("fdmax == %d\n", fdmax);
		 if(newsockfd > fdmax) { 
		 printf("newsockfd > fdmax.\n");
		 printf("fdmax == newsockfd.\n");
		 fdmax = newsockfd;
		 } 

		 if(newsockfd < fdmax) {
		 printf("newsockfd < fdmax.\n");
		 }

		 if(newsockfd == fdmax) {
		 printf("newsockfd == fdmax.\n");
		 }

		printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(cli_addr.sin_addr), newsockfd);
		}
	}

		else { // Start of read loop
		/* handle data from a client */
		bytes_read = recv(i, buffer, sizeof(buffer), 0);
		 if(bytes_read <= 0) {

		/* got error or connection closed by client */
		 if(bytes_read == 0) {
		 /* connection closed */
		 printf("%s: socket %d hung up\n", argv[0], i);
		 } 

		 	else { 
			printf("RECV() error --> %s.\n", strerror(errno));
			}

		 close(i);
		 FD_CLR(i, &master);		
		}

		else { // Start of inner read loop if true
		for(j = 0; j <= fdmax; j++) { // Start of inner for loop to send data
		/* send to everyone! */

		if(FD_ISSET(j, &master)) {

		printf("j is in the masterset.\n");
  		/* except the listener and ourselves */
		if(j == sockfd) {
		printf("j == sockfd DEBUG MSG --> %s.\n", strerror(errno));
		}
		
		if(j != sockfd) {
		printf("j != sockfd DEBUG MSG --> %s.\n", strerror(errno));
		}

		if(j == i) {
		printf("j == i DEBUG MSG --> %s.\n", strerror(errno));
		}

		if(j != i) {
		printf("j != i DEBUG MSG --> %s.\n", strerror(errno));
		}

		  if((j != sockfd) && (j != i)) {
		  printf("j != sockfd && j != i");
  	  	  bytes_written = send(j, buffer, bytes_read, 0); 
		  if(bytes_written == -1) {
  	  	  printf("WRITE(-1) error --> %s.\n", strerror(errno));
		  }

		  if(bytes_written == 0) {
		  printf("WRITE(0) error --> %s.\n", strerror(errno));
		  }

		  printf("WRITE(1) was successful.\n");

  		 }
		}
		} // End of inner for loop to send data
		} // Inner read if read > 0
	} // End of read loop
	}
	}
}

return 0;
}
