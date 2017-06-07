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

int main(int argc, char *argv[]) {
struct sockaddr_in serv_addr, cli_addr;
struct hostent *server;

int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd) {
	  do {
	 	  {
	 	    printf("Waiting for a connection...\n");
	 	  }
	  } while(!accept);
  }

int yes = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
server = gethostbyname(argv[1]);

int portno = atoi(argv[2]);
bzero((char *)&serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
serv_addr.sin_port = htons(portno);

int binder = bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
int listener = listen(sockfd, 20);

socklen_t clilen = sizeof(cli_addr);
int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

for(;;) {
  char buffer[4096];

  ssize_t bytes_read = recv(newsockfd, buffer, sizeof(buffer), 0);

	if((bytes_read) && (debug == 1)) {
	  printf("RECV was successful.\n");
	}

	printf("Client: %s", buffer);

  const char message[] = "Message recieved.\n";
  ssize_t bytes_written = send(newsockfd, message, strlen(message), 0);

  if((bytes_written) && (debug == 1)) {
    printf("SEND(1) - Message sent.\n");
  }

}

close(sockfd);
close(newsockfd);

return 0;
}
