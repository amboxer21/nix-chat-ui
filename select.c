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
fd_set master;
// the temorary fd list for select call
fd_set read_fds;

struct sockaddr_in serv_addr, cli_addr;
struct hostent *server;
int fdmax;
int i, j;
FD_ZERO(&master);
FD_ZERO(&read_fds);

	if((argc < 3) && (argc > 4)) {
	printf("USAGE: %s + IP Address + Port No.\n", argv[0]);
	exit(EXIT_FAILURE);
	}

int debug;
	if(argc == 4) {
	debug = atoi(argv[3]);
	}

/* get the listener */
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
	printf("SOCKET(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}
		else if((sockfd) && (debug == 1)) {
		printf("Successfully created socket.\n");
		}
int yes = 1;
int sockopt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if(sockopt == -1) {
	printf("SETSOCKOPT(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}
		else if((sockopt) && (debug == 1)){
		printf("Setsockopt() was successful.\n");
		}

server = gethostbyname(argv[1]); 
	if(server == NULL) {
	printf("GETHOSTBYNAME(NULL) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}

		else if((server) && (debug == 1)) {
		printf("Successfuly got host by name.\n");
		}

int portno = atoi(argv[2]);
	if(portno < 0) {
	printf("ATOI(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}

	if((portno == 0) && (debug == 1)) {
	printf("ATOI(0) - DEBUG MSG --> %s.\n", strerror(errno));
	}

	if((portno) && (debug == 1)) {
	printf("Successfully binded to port %d.\n", portno);
	}

serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
serv_addr.sin_port = htons(portno);
memset(&(serv_addr.sin_zero), '\0', 8);

int binder = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(binder < 0) {
	printf("BIND(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}

	if((binder == 0) && (debug == 1)) {
	printf("BIND(0) - DEBUG MSG --> %s.\n", strerror(errno));
	}
	
	if((binder) && (debug == 1)) {
	printf("Bind was successful.\n");
	}

int listener = listen(sockfd, 10);
	if(listener < 0) {
	printf("Listen(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}

	if((listener == 0) && (debug == 1)) {
	printf("LISTEN(0) - DEBUG MSG --> %s.\n", strerror(errno));
	}
		else {
		printf("Listening for incoming connections.\n");
		 if((listener) && (debug == 1)) {
		 printf("Entering main loop.\n");
		 }
		}
 
FD_SET(sockfd, &master);
//Watch the current FD.
fdmax = sockfd; 

for( ; ; ) {
read_fds = master;
char buffer[4096];

int selector = select(fdmax+1, &read_fds, NULL, NULL, NULL);
	if(selector < 0) {
	printf("SELECT(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}

		else {
		printf("\n\nSelect() was successful.\n");
		}

// Run through the existing connections looking for data to be read
for(i = 0; i <= fdmax; i++) {
	if(FD_ISSET(i, &read_fds)) { 

	if(i == sockfd) {
	 if(debug == 1) {
	 printf("i == sockfd DEBUG MSG --> %s.\n", strerror(errno));
	 }

	socklen_t clilen;
	clilen = sizeof(cli_addr);

	int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	if(newsockfd < 0) {
	printf("Accept(-1) error --> %s.\n", strerror(errno));
	exit(EXIT_FAILURE);
	}

	if((newsockfd == 0) && (debug == 1)) {
	printf("ACCEPT(0) - DEBUG MSG --> %s.\n", strerror(errno));
	}

		else {
		 if(debug == 1) {
		 printf("Accept(1) was successful.\n");
		 }

		 // Add new FD to masterset.
		 FD_SET(newsockfd, &master); 
		 
		 if(debug == 1) {
		 printf("newsockfd == %d\n", newsockfd);
		 printf("fdmax == %d\n", fdmax);
		 }

		 if(newsockfd > fdmax) { 
		  if(debug == 1) {
		  printf("newsockfd > fdmax.\n");
		  printf("fdmax == newsockfd.\n");
		  }

		 fdmax = newsockfd;
		 } 

		 if((newsockfd < fdmax) && (debug == 1)) {
		 printf("newsockfd < fdmax.\n");
		 }

		 if((newsockfd == fdmax) && (debug == 1)) {
		 printf("newsockfd == fdmax.\n");
		 }

		printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(cli_addr.sin_addr), newsockfd);
		}
	}

		else { // Start of read loop
		ssize_t bytes_read = recv(i, buffer, sizeof(buffer), 0);
		 if(bytes_read <= 0) {

		 if(bytes_read < 0) {
		 printf("RECV(-1) error --> %s.\n", strerror(errno));
		 exit(EXIT_FAILURE);
		 }

		 if(bytes_read == 0) {
		 // Connection closed by remote host
		 printf("%s: socket %d hung up\n", argv[0], i);
		 }

		 FD_CLR(i, &master);
		 close(i);
		}

		else { // Start of inner read loop if true
		 if((bytes_read) && (debug == 1)) {
		 printf("Recieve was successful.\n");
		 }

		 FD_CLR(i, &master);
		 close(i);

		for(j = 0; j <= fdmax; j++) { // Start of inner for loop to send data
		// Echo message to all
		if(FD_ISSET(j, &read_fds)) {

		 if(debug == 1) {
		 printf("j is in the masterset.\n");
		 }

		if((j == sockfd) && (debug == 1)) {
		printf("j == sockfd DEBUG MSG --> %s.\n", strerror(errno));
		}

		if((j != sockfd) && (debug == 1)) {
		printf("j != sockfd DEBUG MSG --> %s.\n", strerror(errno));
		}

		if((j == i) && (debug == 1)) {
		printf("j == i DEBUG MSG --> %s.\n", strerror(errno));
		}

		if((j != i) && (debug == 1)) {
		printf("j != i DEBUG MSG --> %s.\n", strerror(errno));
		}

  		// Except the listener and ourselves 
		  if((j != sockfd) && (j != i)) {
		   if(debug == 1) {
		   printf("j != sockfd && j != i");
		   }

  	  	  ssize_t bytes_written = send(j, buffer, bytes_read, 0);
		  if(bytes_written < 0) {
  	  	  printf("WRITE(-1) error --> %s.\n", strerror(errno));
		  }

		  if((bytes_written == 0) && (debug == 1)) {
		  printf("WRITE(0) - DEBUG MSG --> %s.\n", strerror(errno));
		  }

		  if((bytes_written) && (debug == 1)) {
		  printf("WRITE(1) was successful.\n");
		  }

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
