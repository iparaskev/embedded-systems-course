#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "constants.h"


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

	int numbytes;
	/* Send address to server.*/ 
	char from[ADDRESS_SIZE];
	fgets(from, ADDRESS_SIZE, stdin);
	if((numbytes = send(socket_fd, from, strlen(from), 0)) == -1)
		perror("send");

	/* Get incoming messages.*/
	char message[MAXDATA_SIZE];
	while (1)
	{
		numbytes = recv(socket_fd, message, MAXDATA_SIZE - 1, 0);
		if (numbytes == -1)
			perror("receive");
		message[numbytes] = '\0';
		if (strcmp(message, "\r\n") == 0)
			break;
		printf("Message: %s", message);
	}

	/* Send Address of receiver.*/
	char to[ADDRESS_SIZE];
	fgets(to, ADDRESS_SIZE, stdin);
	if((numbytes = send(socket_fd, to, strlen(to), 0)) == -1)
		perror("send");

	/* Send message*/
	if (strcmp(to, "\r\n") != 0)
	{
		fgets(message, MAXDATA_SIZE, stdin);
		numbytes = send(socket_fd, message, strlen(message), 0);
		if(numbytes == -1)
			perror("send");
	}
	close(socket_fd);
	return 0;
}
