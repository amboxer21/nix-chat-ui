#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFF_SIZE 4096

int main(int argc, char *argv[]) {

  struct hostent *server;
  struct sockaddr_in serv_addr, cli_addr;

	if((argc < 3) || (argc > 4)) {
	  printf("USAGE: %s + IP Addr + Portno.\n", argv[0]);
	  exit(EXIT_FAILURE);
	}

  int debug;
	if(argc == 4) {
	  debug = atoi(argv[3]);
	}

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
	  printf("SOCKET(-1) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

	if((sockfd == 0) && (debug == 1)) {
	  printf("SOCKET(0) - DEBUG MSG --> %s.\n", strerror(errno));
	}

  else if(sockfd) {
	  do {
	 	  {
	 	    printf("Waiting for a connection...\n");
	 	  }
	 	} while(!accept);
	}

  int yes = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  server = gethostbyname(argv[1]);

	if(server == NULL) {
	  printf("GETHOSTBYNAME(NULL) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

	if((server != NULL) && (debug == 1)) {
	  printf("Sucsessfully got host by name.\n");
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
	  printf("ATOI(1) - DEBUG MSG --> %s.\n", strerror(errno));
	}

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  serv_addr.sin_port = htons(portno);

  int binder = bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if(binder < 0) {
	  printf("BIND(-1) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

  if((binder == 0) && (debug == 1)) {
    printf("BIND(0) - DEBUG MSG --> %s.\n", strerror(errno));
	}

  if((binder) && (debug == 1)) {
    printf("BIND(1) - DEBUG MSG --> %s.\n", strerror(errno));
  }

  int listener = listen(sockfd, 20);
	if(listener < 0) {
	  printf("LISTEN(-1) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

  if((listener == 0) && (debug == 1)) {
    printf("LISTEN(0) - DEBUG MSG --> %s.\n", strerror(errno));
	}

  if((listener) && (debug == 1)) {
    printf("LISTEN(1) - DEBUG MSG --> %s.\n", strerror(errno));
  }

  socklen_t clilen;
  clilen = sizeof(cli_addr);

  int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	if(newsockfd < 0) {
	  printf("ACCEPT(-1) error --> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

	if((newsockfd == 0) && (debug == 1)) {
	  printf("ACCEPT(0) - DEBUG MSG --> %s.\n", strerror(errno));
	}

	if((newsockfd) && (debug == 1)) {
	  printf("ACCEPTED new connection.\n");
	}

  for(;;) {
    char buffer[BUFF_SIZE];

    ssize_t bytes_read = recv(newsockfd, buffer, sizeof(buffer), 0);
	  if(bytes_read < 0) {
	    printf("RECV(-1) error --> %s.\n", strerror(errno));
	    exit(EXIT_FAILURE);
	  }

	  if((bytes_read == 0) && (debug == 1)) {
	    printf("Connection closed.\n");
	  }

	  if((bytes_read) && (debug == 1)) {
	    printf("RECV was successful.\n");
	  }

	  printf("Client: %s\n", buffer);

    const char message[] = "Message recieved.\n";
    ssize_t bytes_written = send(newsockfd, message, strlen(message), 0);
	  if(bytes_written < 0) {
	    printf("SEND(-1) error --> %s.\n", strerror(errno));
	    exit(EXIT_FAILURE);
	  }

    if((bytes_written == 0) && (debug == 1)) {
      printf("SEND(0) - DEBUG MSG --> %s.\n", strerror(errno));
	  }

    if((bytes_written) && (debug == 1)) {
      printf("SEND(1) - Message sent.\n");
    }

  }

  close(sockfd);
  close(newsockfd);

  return 0;
}
