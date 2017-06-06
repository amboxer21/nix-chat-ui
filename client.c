#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char *argv[]) {  

  struct hostent *server; 
  struct sockaddr_in serv_addr/*, cli_addr*/;

	if((argc < 3) && (argc > 4)) {
	  fprintf(stderr, "Ussage: %s + IP Address + port No. Append a 1 to turn verbose on.\n", argv[0]);
	  exit(EXIT_FAILURE);
	}

  int debug;
	if(argc == 4) {
	  debug = atoi(argv[3]);
	}


  int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if(sockfd < 0) {
	  printf("SOCKET(-1) error ---> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

	if((sockfd == 0) && (debug == 1)) {
	  printf("SOCKET(0) error ---> %s.\n", strerror(errno));
	}

	if((sockfd) && (debug == 1)) {
	  printf("DEBUG MSG --> SOCKET was successful.\n");
	}

  int yes = 1; 
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  bzero((char *) &serv_addr, sizeof(serv_addr)); 
  server = gethostbyname(argv[1]); 
	if(server == NULL) {
	  fprintf(stderr, "No such host.\n");
	  printf("%s\n", strerror(errno)); 
	  exit(EXIT_FAILURE);
	}

  else if((server) && (debug == 1)) {
		printf("DEBUG MSG - Successfully got host by name.\n");
	}

  int portno = atoi(argv[2]);
	if(portno < 0) {
	  printf("PORTNO(0) error ---> %s.\n", strerror(errno));
	}

	if((portno == 0) && (debug == 1)) {
	  printf("DEBUG MSG - ATOI(0) error ---> %s.\n", strerror(errno));
	}

	if((portno) && (debug == 1)) {
	  printf("Successfully binded to port %d.\n", portno);
	}

  serv_addr.sin_family = AF_INET;
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length); 
  serv_addr.sin_port = htons(portno);

  int connector = connect(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(connector < 0) { 
	  fprintf(stderr, "%s. CONNECT()\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

	else {
		printf("Made a connection to %s\n", inet_ntoa(serv_addr.sin_addr)); 
	}

  for( ; ; ) {
    char buffer[BUFF_SIZE];

    printf("Message: ");
    fgets(buffer, sizeof(buffer), stdin);
    ssize_t bytes_written = write(sockfd, buffer, strlen(buffer));
	  if(bytes_written < 0) { 
	    printf("WRITE(-1) error ---> %s.\n", strerror(errno));
	  }

	  if(bytes_written == 0) {
	    //printf("WRITE(0) error ---> %s.\n", strerror(errno));
	    printf("Nothing was written.\n");
	  }

	  if((bytes_written) && (debug == 1)) { 
	    printf("DEBUG MSG - WRITE was successful.\n");
	  }

    ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer));
	  if(bytes_read < 0) {
      //fprintf(stderr, "Error reading message from %s\n", inet_ntoa(cli_addr.sin_addr));
      printf("READ(-1) error ---> %s.\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

	  //Test to see if the buffer is blank.
    if((bytes_read == 0) && (debug == 1)) {
      printf("READ(0) error ---> %s.\n", strerror(errno));
    }
		else {
		  printf("server: %s", buffer);
		}

  }

  close(sockfd);
  return 0;
}
