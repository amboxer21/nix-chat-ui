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

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {

  fd_set master, read_fds;

  struct hostent *server;
  struct in_addr* ip_address;
  struct sockaddr_in serv_addr, cli_addr;

  int fdmax, i;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  server = gethostbyname("0.0.0.0");
  ip_address = (struct in_addr*)server->h_addr_list[0];

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
	  printf("SOCKET(-1) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

  int yes = 1;
  int sockopt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if(sockopt == -1) {
	  printf("SETSOCKOPT(-1) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = ip_address->s_addr;
  //memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  serv_addr.sin_port = htons(3345);
  memset(&(serv_addr.sin_zero), '\0', 8);

  //Bind() returns zero upon success.
  int binder = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(binder < 0) {
	  printf("BIND(-1) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

  int listener = listen(sockfd, 10);
	if(listener < 0) {
	  printf("Listen(-1) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}
  //Listen() return zero upon success.
	else {
		printf("Listening for incoming connections.\n");
	}

  FD_SET(sockfd, &master);
  fdmax = sockfd; 

  for( ; ; ) {
    read_fds = master;
    char buffer[BUFFER_SIZE];
    const char newline[] = "\n";

    int selector = select(fdmax+1, &read_fds, NULL, NULL, NULL);
	  if(selector < 0) {
	    printf("SELECT(-1) error --> %s.\n", strerror(errno));
	    exit(EXIT_FAILURE);
	  }

    for(i = 0; i <= fdmax; i++) {
	    if(FD_ISSET(i, &read_fds)) {

	      if(i == sockfd) {
	        socklen_t clilen;
	        clilen = sizeof(cli_addr);

	        int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	        if(newsockfd < 0) {
	          printf("Accept(-1) error --> %s.\n", strerror(errno));
	          exit(EXIT_FAILURE);
	        }

	        if(newsockfd == 0) {
	          printf("ACCEPT(0) - DEBUG MSG --> %s.\n", strerror(errno));
	        }
	        
	    	  else {
	    	    FD_SET(newsockfd, &master); 
	    	    if(newsockfd > fdmax) {
	    	      fdmax = newsockfd;
	    	    }
	
	    	    printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(cli_addr.sin_addr), newsockfd);
	    	  }
	      }
	
	  	  else {
	  	    ssize_t bytes_read = recv(i, buffer, sizeof(buffer), 0);
	  	    if(bytes_read <= 0) {

	  	      if(bytes_read < 0) {
	  	        printf("RECV(-1) error --> %s.\n", strerror(errno));
	  	        exit(EXIT_FAILURE);
	  	      }

	  	      if(bytes_read == 0) {
	  	        printf("%s: socket %d hung up\n", argv[0], i);
	  	        FD_CLR(i, &master);
	  	        close(i);
	  	      }
	  	    }

	  		  else if(bytes_read) {
            const char newline[] = "\n";
	  		    printf("Recieve was successful.\n");
            if(strncmp(buffer,"display",7) == 0) {
              printf("=> Password accepted: %s",buffer);
            }
	  		    ssize_t bytes_written = send(i, newline, 1, 0);
	  		    if(bytes_written < 0) {
	  		      printf("WRITE(-1) error ---> %s.\n", strerror(errno));
	  		    }

	  		    if(bytes_written == 0) {
	  		      printf("Nothing was written.\n");
	  		    }

	  		    if(bytes_written) {
			        printf("Send was successful.\n");
	  		        memset(buffer, '\0', sizeof(buffer));
	  		    }

	  		  }
	  	  }
	    }
    }
  }

  return 0;
}
