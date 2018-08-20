#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXDATASIZE 256

/*
	socket
	connect
	send
*/

int 
main(int argc, char **argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "ERROR: no port or ip provided\n");
		exit(1);
	}

	struct sockaddr_in server_addr;

	int socket_fd;
	if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("server: socket");
		exit(1);
	} 
	
	// fill server address info
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &(server_addr.sin_addr)) == -1)
	{
		perror("inet_pton");
		exit(-1);
	}
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

	// connect to 
	if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof server_addr) == -1)
	{
		perror("connect");
		exit(-1);
	}

	// send the message
	char buffer[MAXDATASIZE];
	fgets(buffer, MAXDATASIZE, stdin);
	int numbytes;
	if((numbytes = send(socket_fd, buffer, strlen(buffer), 0)) == -1)
		perror("send");

	close(socket_fd);
	return 0;
}
