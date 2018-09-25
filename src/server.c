#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "constants.h"
#include "data_structures.h"

#define LIMIT 4

/* Global variables*/
users *users_list;
fifo *waiting_clients;
int active_clients = 0;
int wait_counter = 0;

/* Mutex for searching the users*/
pthread_mutex_t user_pass = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t active_pass = PTHREAD_MUTEX_INITIALIZER;

// struct for the accepting descriptors
// TODO: try long int pointer and not a struct
// TODO: blank user has blank message

void *
handle_accept(void *fd)
{

	/* Convert socket descriptor*/
	struct accept_d *accept_fd;
	accept_fd = (struct accept_d *) fd;
	int new_fd = accept_fd->new_fd;

	/* The buffers will be needed.*/
	int numbytes;
	char *message = malloc(MAXDATA_SIZE);
	char *from = malloc(ADDRESS_SIZE);
	char *to = malloc(ADDRESS_SIZE);
	char *buffer = malloc(ADDRESS_SIZE + MAXDATA_SIZE);
	char *unread = malloc(BATCH_SIZE);
	memset(unread, 0, BATCH_SIZE);

	/* Read connected address.*/
	if ((numbytes = recv(new_fd, from, ADDRESS_SIZE - 1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	from[numbytes] = '\0';
	//printf("From %s", from);

	/* Sleep interval between after sending a batch.*/
	struct timespec mini_break;
	mini_break.tv_sec = 0;
	mini_break.tv_nsec = 500000;

	/* Send the unread messages to that address.*/
	users *target = find_user(users_list, from);
	int length = 0;

	if (target != NULL)
	{
		pthread_mutex_lock(&(target->user_mutex));
		while (target->head != NULL)
		{
			strcpy(message, target->head->msg);

			/* If the current length plus message length are
			   bigger than batch size send current message
			*/
			if (length + strlen(message) > BATCH_SIZE - 1)
			{
				int check = strlen(unread);
				numbytes = send(new_fd, unread, BATCH_SIZE - 1, 0);
				if (numbytes == -1)
					perror("send");
				
				nanosleep(&mini_break, NULL);
				/* Clean buffer.*/
				memset(unread, 0, BATCH_SIZE);
				length = 0;
			}

			/* Concatenate the message with the others*/
			strcat(unread, message);
			length += strlen(message);
			delete_message(target);
		}
		/* Send the last batch of messages*/
		//printf("User %s, messages %d\n", from, counter);
		pthread_mutex_unlock(&(target->user_mutex));
	}

	/*Send termination message for the uread*/
	strcat(unread, "end_messages_0@#1\n");
	numbytes = send(new_fd, unread, BATCH_SIZE - 1, 0);
	if (numbytes == -1)
		perror("send");
	memset(message, 0, MAXDATA_SIZE);

	int safe_counter = 0;
	while (1)
	{
		/* Receive message*/
		numbytes = recv(new_fd, buffer, MAXDATA_SIZE + ADDRESS_SIZE -1, 0);
		if ( numbytes == -1)
		{
			perror("recv");
			exit(1);
		}
		buffer[numbytes] = '\0';

		char *no_receiver = {"\n"};
		if (strcmp(buffer, no_receiver) != 0)
		{
			/* Extract receiver address*/
			int new_line = 0;
			int counter_new_lines = 0;
			int i;
			for (i = 0; i < numbytes - 1; i++)
			{
				if ((buffer[i] != '\n') && !counter_new_lines)
					to[i] = buffer[i];	
				else if (buffer[i] != '\n' && counter_new_lines < 2)
					message[i - new_line] = buffer[i];
				else
				{
					if (!counter_new_lines)
						new_line = i + 1;
					counter_new_lines++;
					to[i] = '\n';
					to[i + 1] = '\0';
				}
			}
			/* Clear buffer for next message*/
			memset(buffer, 0, MAXDATA_SIZE + ADDRESS_SIZE);

			message[numbytes - new_line - 1] = '\n';
			message[numbytes - new_line] = '\0';
			//printf("To: %sMessage: %s", to, message);

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
		}
		else
			break;
		safe_counter++;
		if (safe_counter == 10000)
			break;
		/* Only for testing*/
		break;
	}

	close(new_fd);
	free(accept_fd);

	/* Update active connections counter.*/
	pthread_mutex_lock(&active_pass);
	if (wait_counter > 0)
	{
		fifo *c = waiting_clients;

		/* Read the first socket descriptor from the fifo.*/
		struct accept_d *input = malloc(sizeof *input);
		input = waiting_clients->sd;

		/*Create new thread from list*/
		pthread_t thread;
		pthread_create(&thread, NULL, handle_accept, (void *) input);

		/* Delete the given descriptor*/
		if (waiting_clients->next != NULL)
			delete_descriptor(waiting_clients);
		else
		{
			free(waiting_clients);
			waiting_clients = NULL;
		}

		/* Decrease the wait_counter.*/
		wait_counter--;
	}
	else
		active_clients--;
	
	//printf("Thread:\nactive cliens %d\nwaiting clients: %d\n", active_clients, wait_counter);
	pthread_mutex_unlock(&active_pass);

	/* Detach the thread, so we dont have zombies*/
	pthread_detach(pthread_self());

	/* Clean up before exit*/
	free(unread);
	free(message);
	free(from);
	free(to);
	free(buffer);

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

	/*Initialize the fifo for the waiting clients*/
	waiting_clients = NULL;

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

		pthread_mutex_lock(&active_pass);
		//printf("%d\n", wait_counter);
		if (active_clients < LIMIT)
		{
			active_clients++;

			/* Create new thread*/
			pthread_t thread;
			pthread_create(&thread, NULL, handle_accept, (void *) fd);
		}
		else
		{
			/* Append an element to the waiting list.*/
			if (waiting_clients != NULL)
			{
				add_descriptor(waiting_clients, fd);
			}
			else
			{
				waiting_clients = malloc(sizeof *waiting_clients);
				if (waiting_clients == NULL)
				{
					perror("malloc");
					exit(1);
				}
				waiting_clients->next = NULL;
				waiting_clients->sd = fd;
			}
			wait_counter++;

		}
		//printf("Main:\nactive cliens %d\nwaiting clients: %d\n", active_clients, wait_counter);
		pthread_mutex_unlock(&active_pass);
	}
	
	return 0;
}
