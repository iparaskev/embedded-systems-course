#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "constants.h"
#include "data_structures.h"

// TODO Reap zombie threads

/* Global variables*/
users *users_list;

/* Mutex for searching the users*/
pthread_mutex_t user_pass = PTHREAD_MUTEX_INITIALIZER;

// struct for the accepting descriptors
// TODO: try long int pointer and not a struct
struct accept_d
{
	int new_fd;
};

void *
handle_accept(void *fd)
{
	/* Convert socket descriptor*/
	struct accept_d *accept_fd;
	accept_fd = (struct accept_d *) fd;
	int new_fd = accept_fd->new_fd;

	/* The buffers will be needed.*/
	int numbytes;
	char message[MAXDATA_SIZE];
	char from[ADDRESS_SIZE];
	char to[ADDRESS_SIZE];
	char buffer[ADDRESS_SIZE + MAXDATA_SIZE];

	/* Read connected address.*/
	if ((numbytes = recv(new_fd, from, ADDRESS_SIZE - 1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	from[numbytes] = '\0';
	//printf("from: %s", from);

	/* Send the unread messages to that address.*/
	users *target = find_user(users_list, from);
	if (target != NULL)
	{
		pthread_mutex_lock(&(target->user_mutex));
		while (target->head != NULL)
		{
			strcpy(message, target->head->msg);
			numbytes = send(new_fd, message, MAXDATA_SIZE - 1, 0);
			if (numbytes == -1)
				perror("send");
			delete_message(target);
		}
		pthread_mutex_unlock(&(target->user_mutex));
	}

	/*Send termination message for the uread*/
	strcpy(message, "\r\n");
	numbytes = send(new_fd, message, MAXDATA_SIZE - 1, 0);
	if (numbytes == -1)
		perror("send");
	memset(message, 0, MAXDATA_SIZE);

	/* Receive message*/
	if ((numbytes = recv(new_fd, buffer, MAXDATA_SIZE + ADDRESS_SIZE -1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	buffer[numbytes] = '\0';

	/* Extract receiver address*/
	int new_line = 0;
	for (int i = 0; i < numbytes - 1; i++)
	{
		if ((buffer[i] != '\n') && !new_line)
			to[i] = buffer[i];	
		else if (buffer[i] != '\n' && new_line)
			message[i - new_line] = buffer[i];
		else
		{
			new_line = i + 1;
			to[i] = '\n';
			to[i + 1] = '\0';
		}
	}
	message[numbytes - new_line - 1] = '\n';
	message[numbytes - new_line] = '\0';
	//printf("to: %smessage: %s\n", to, message);

	/* Append the message to the list with the other messages*/
	pthread_mutex_lock(&user_pass);
	target = find_user(users_list, to);
	if (target == NULL)
	{
		add_user(users_list, to, message);
		pthread_mutex_unlock(&user_pass);
	}
	else
	{
		pthread_mutex_unlock(&user_pass);
		pthread_mutex_lock(&(target->user_mutex));
		add_message(target, message);
		pthread_mutex_unlock(&(target->user_mutex));
	}

	close(new_fd);
	free(accept_fd);
	pthread_exit(NULL);
}

int 
main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "ERROR: no port provided\n");
		exit(1);
	}

	/* 
		socket
		bind 
		listen
		accept
		recv
		close
	*/

	int listen_fd, new_fd; // listening and new connection descriptor
	struct sockaddr_in server_addr;
	struct sockaddr_storage client_addr;

	// fill server address info
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = INADDR_ANY; // bind my local ip
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

	// open a socket descriptor for listening
	if ((listen_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("server: socket");
		exit(1);
	}

	// bind the port of server_addr to the listen_fd 
	if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof server_addr) == -1)
	{
		close(listen_fd);
		perror("server: bind");
		exit(1);
	}

	// start listening for incoming connections
	if (listen(listen_fd, BACKLOG) == -1)
	{
		close(listen_fd);
		perror("server: listen");
		exit(1);
	}

	socklen_t addr_size = sizeof client_addr;
	struct accept_d *fd;

	/* Initialize the users list*/
	users_list = malloc(sizeof *users_list);
	if (users_list == NULL)
	{
		perror("malloc");
		exit(1);
	}
	users_list->name = "init_USER_0";
	users_list->head = NULL;
	users_list->next = NULL;

	// new process for every connection
	while(1)
	{
		// accept connection
		new_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &addr_size);
		if (new_fd == -1) 
		{
			perror("accept");
			continue;
		}

		fd = malloc(sizeof (struct accept_d));
		fd->new_fd = new_fd;
		pthread_t thread;
		pthread_create(&thread, NULL, handle_accept, (void *) fd);
		
	}
	
	return 0;
}
