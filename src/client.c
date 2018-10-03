#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
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

	/* Parse arguments*/
	int opt;
	int testing = 0;
	char *optstring = "t";
	while ((opt = getopt(argc, argv, optstring)) != -1)
		switch (opt)
		{
			case 't':
				testing = 1;
				break;
		}

	// connect to 
	while (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof server_addr) == -1)
	{
		if (errno != ETIMEDOUT)
		{
			perror("connect");
			exit(-1);
		}
	}

	int numbytes;
	int bytes;

	if (!testing)
		printf("Type your address: ");

	/* Send address to server.*/ 
	char from[ADDRESS_SIZE];
	fgets(from, ADDRESS_SIZE, stdin);
	if((numbytes = send(socket_fd, from, strlen(from), 0)) == -1)
		perror("send");

	/* Confirmation buffer*/
	char confirm[ACK_SIZE];
	memset(confirm, 0, ACK_SIZE);

	/* Get incoming messages.*/
	if (!testing)
		printf("Unread messages\n");
	char unread[BATCH_SIZE];
	char message[MAXDATA_SIZE];
	memset(message, 0, MAXDATA_SIZE);
	int flag = 0;
	while (1)
	{
		numbytes = recv(socket_fd, unread, BATCH_SIZE - 1, 0);
		if (numbytes == -1)
			perror("receive");
		unread[numbytes] = '\0';

		/* Send confirmation.*/
		bytes = send(socket_fd, confirm, ACK_SIZE - 1, 0);
		if (numbytes == -1)
			perror("send");

		/* Split the messages. */
		int index = 0;
		for (int byte = 0; byte < numbytes; byte++)
		{
			if (unread[byte] != 10)
			{
				message[index] = unread[byte];
				index++;
			}
			else
			{
				if (strcmp(message, "end_messages_0@#1") == 0)
				{
					flag = 1;
					break;
				}
				printf("Message: %s\n", message);
				memset(message, 0, MAXDATA_SIZE);
				index = 0;
			}
		}
		if (flag)
			break;
	}

	
	/* Buffers for sending the messages*/
	char batch[BATCH_SIZE];      // Buffer for sending batches of messages
	memset(batch, 0, BATCH_SIZE);

	char to[ADDRESS_SIZE];       // Buffer for geting the receiver address

	int batch_length = 0;
	char *terminal_string = "\n";
	while (1)
	{
		/* Send address of receiver and message.*/
		memset(to, 0, ADDRESS_SIZE);
		if (!testing)
			printf("Type receiver's address or for termination press enter: ");
		fgets(to, ADDRESS_SIZE, stdin);

		/* Send message*/
		if (strcmp(to, "\n") != 0)
		{
			/* Get message from stdin*/
			memset(message, 0, MAXDATA_SIZE);
			if (!testing)
				printf("Type message: ");
			fgets(message, MAXDATA_SIZE, stdin);

			if ((strlen(message) + strlen(to) + batch_length) > BATCH_SIZE - 1)
			{
				//printf("Length %d\n", batch_length);
				numbytes = send(socket_fd, batch, batch_length, 0);
				if (numbytes == -1)
					perror("Send: ");

				/* Get confirmation.*/
				numbytes = recv(socket_fd, confirm, ACK_SIZE - 1, 0);
				if (numbytes == -1)
					perror("receive");

				/* Clear the buffer and the length*/
				memset(batch, 0, BATCH_SIZE);
				batch_length = 0;
			}

			/* Concatenate message and to to batch.*/
			strcat(batch, to);
			strcat(batch, message);
			batch_length += strlen(to) + strlen(message);
		}
		else
		{
			/* Send the last messages*/
			if (batch_length < 1)
				batch_length = 1;
			numbytes = send(socket_fd, batch, batch_length, 0);
			if (numbytes == -1)
				perror("Send: ");
			memset(batch, 0, BATCH_SIZE);

			/* Get confirmation.*/
			numbytes = recv(socket_fd, confirm, ACK_SIZE - 1, 0);
			if (numbytes == -1)
				perror("receive");
			//puts("Got 1");

			/* Send the termination characters.*/
			strcpy(batch, terminal_string);
			batch_length = strlen(batch);
			numbytes = send(socket_fd, batch, batch_length, 0);
			if (numbytes == -1)
				perror("Send: ");

			/* Get confirmation.*/
			numbytes = recv(socket_fd, confirm, ACK_SIZE - 1, 0);
			if (numbytes == -1)
				perror("receive");
			//puts("Got 2");
			break;
		}
	}

	close(socket_fd);
	return 0;
}
