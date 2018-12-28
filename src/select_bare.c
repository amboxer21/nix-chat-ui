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
  fd_set master, read_fds;

  struct sockaddr_in serv_addr, cli_addr;
  struct hostent *server;
  int fdmax, i;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);

	if(argc < 3) {
	  printf("USAGE: %s + IP Address + Port No.\n", argv[0]);
	  exit(EXIT_FAILURE);
	}

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

  server = gethostbyname(argv[1]); 
	if(server == NULL) {
	  printf("GETHOSTBYNAME(NULL) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

  int portno = atoi(argv[2]);
	if(portno < 0) {
	  printf("ATOI(-1) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

	if(portno == 0) {
	  printf("ATOI(0) - DEBUG MSG --> %s.\n", strerror(errno));
	}

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  //memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
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
    char buffer[4096];

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
            char temp[1024];
            stpncpy(temp,(char *)"\n",strlen(temp));
	  		    printf("Recieve was successful.\n");
            if(strncmp(buffer,"display",7) == 0) {
              printf("=> Password accepted: %s",buffer);
            }
	  		    ssize_t bytes_written = send(i, temp, 1, 0);
	  		    if(bytes_written < 0) {
	  		      printf("WRITE(-1) error ---> %s.\n", strerror(errno));
	  		    }

	  		    if(bytes_written == 0) {
	  		      printf("Nothing was written.\n");
	  		    }

	  		    if(bytes_written) {
			        printf("Send was successful.\n");
	  		        memset(buffer, '\0', strlen(buffer));
	  		        memset(temp, '\0', strlen(temp));
	  		    }

	  		  }
	  	  }
	    }
    }
  }

  return 0;
}
