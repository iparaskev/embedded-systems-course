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

#define LIMIT 16

/* Global variables*/
users *users_list;
fifo *waiting_clients;
int active_clients = 0;
int wait_counter = 0;
int handled = 0;
int testing = 0;

/* Mutex for searching the users*/
pthread_mutex_t user_pass = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t active_pass = PTHREAD_MUTEX_INITIALIZER;


void *
handle_accept(void *fd)
{
	/* Convert socket descriptor*/
	struct accept_d *accept_fd;
	accept_fd = (struct accept_d *) fd;
	int new_fd = accept_fd->new_fd;

	/* The buffers will be needed.*/
	int numbytes;
	int bytes;
	char *message = malloc(MAXDATA_SIZE);
	if (message == NULL)
	{
		perror("malloc at message: ");
		exit(1);
	}

	char *from = malloc(ADDRESS_SIZE);
	if (from == NULL)
	{
		perror("malloc at from: ");
		exit(1);
	}
	
	char *to = malloc(ADDRESS_SIZE);
	if (to == NULL)
	{
		perror("malloc at to: ");
		exit(1);
	}

	char *buffer = malloc(BATCH_SIZE);
	if (buffer == NULL)
	{
		perror("malloc at buffer: ");
		exit(1);
	}

	char *unread = malloc(BATCH_SIZE);
	if (unread == NULL)
	{
		perror("malloc at unread: ");
		exit(1);
	}
	memset(unread, 0, BATCH_SIZE);

	char *confirm = malloc(ACK_SIZE);
	if (confirm == NULL)
	{
		perror("malloc at confirmation: ");
		exit(1);
	}
	memset(confirm, 0, ACK_SIZE);

	/* Read connected address.*/
	if ((numbytes = recv(new_fd, from, ADDRESS_SIZE - 1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	from[numbytes] = '\0';

	/* Send the unread messages to that address.*/
	users *target = find_user(users_list, from);
	int length = 0;     // The batch's running length
	int counter = 0;
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
				
				/* Get confirmation.*/
				numbytes = recv(new_fd, confirm, ACK_SIZE - 1, 0);
				if (numbytes == -1)
					perror("receive");

				//nanosleep(&mini_break, NULL);
				/* Clean buffer.*/
				memset(unread, 0, BATCH_SIZE);
				length = 0;
			}

			/* Concatenate the message with the others*/
			strcat(unread, message);
			length += strlen(message);
			delete_message(target);
			counter++;
		}
		printf("Counter %d\n", counter);
		pthread_mutex_unlock(&(target->user_mutex));
	}

	int checker = strlen(unread);
	/* Send last batch.*/
	numbytes = send(new_fd, unread, BATCH_SIZE - 1, 0);
	if (numbytes == -1)
		perror("send");
	if (numbytes < checker)
		printf("Sended %d, message wa %d\n", numbytes, checker);

	/* Get confirmation.*/
	numbytes = recv(new_fd, confirm, ACK_SIZE - 1, 0);
	if (numbytes == -1)
		perror("receive");
	memset(unread, 0, BATCH_SIZE);

	/*Send termination message for the uread*/
	strcat(unread, "end_messages_0@#1\n");
	checker = strlen(unread);
	numbytes = send(new_fd, unread, BATCH_SIZE - 1, 0);
	if (numbytes == -1)
		perror("send");
	if (numbytes < checker)
		printf("2 Sended %d, message  %d\n", numbytes, checker);
	/* Get confirmation.*/
	numbytes = recv(new_fd, confirm, ACK_SIZE - 1, 0);
	if (numbytes == -1)
		perror("receive");

	/* Clear the buffers for receiving messages*/
	memset(message, 0, MAXDATA_SIZE);
	memset(to, 0, ADDRESS_SIZE);

	int flag = 0;            // Flag for terminating the loop
	int new_lines = 0;       // New lines counter
	int to_index = 0;        // The index of receiver's address buffer
	int message_index = 0;   // The index of messsage's buffer
	while (1)
	{
		/* Receive batch*/
		memset(buffer, 0, BATCH_SIZE);
		numbytes = recv(new_fd, buffer, BATCH_SIZE - 1, 0);
		//printf("Numbytes %d\n", numbytes);
		//printf("Message %s", buffer);
		if ( numbytes == -1)
		{
			perror("recv");
			exit(1);
		}
		buffer[numbytes] = '\0';
		//puts("Got");
		/* Send confirmation.*/
		bytes = send(new_fd, confirm, ACK_SIZE - 1, 0);
		if (numbytes == -1)
			perror("send");

		/* Split the messages*/
		if (buffer[0] == '\0')
			continue;
		int byte = 0;
		for (byte; byte < numbytes; byte++)
		{

			/* If the character is not new line add to buffers*/
			if (!(new_lines % 2))
			{
				to[to_index] = buffer[byte];
				to_index++;
			}
			else
			{
				message[message_index] = buffer[byte];
				message_index++;
			}

			/* At new lines update message's lists*/
			//printf("to_index %d t %d b %d\n", to_index, to[0], buffer[byte]);
			if (buffer[byte] == 10)
			{
				//printf("Newline %d\n", new_lines);
				if (!((new_lines) % 2))
				{
					if (strcmp(to, "\n") == 0)
					{
						flag = 1;
						break;
					}
				}
				else 
				{
					/* Add from who is the message*/
					if (!testing)
					{
						int mes_length;
						mes_length = strlen(message);
						message[mes_length - 1] = ' ';
						strcat(message, "from ");
						strcat(message, from);
						
					}
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
					memset(to, 0, ADDRESS_SIZE);
					memset(message, 0, MAXDATA_SIZE);
					to_index = 0;
					message_index = 0;

				}
				new_lines++;
			}
		}
		if (flag)
			break;
	}
	close(new_fd);

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
	pthread_mutex_unlock(&active_pass);

	/* Detach the thread, so we dont have zombies*/
	pthread_detach(pthread_self());

	/* Clean up before exit*/
	free(accept_fd);
	free(unread);
	free(message);
	free(from);
	free(to);
	free(buffer);
	free(confirm);

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

	/* Read stdin*/
	char *optstring = "t";
	int opt;
	while ((opt = getopt(argc, argv, optstring)) != -1)
		switch (opt)
		{
			case 't':
				testing = 1;
				break;
		}


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
	pthread_t thread;
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
		if (active_clients < LIMIT)
		{
			active_clients++;

			/* Create new thread*/
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
		pthread_mutex_unlock(&active_pass);
	}
	
	return 0;
}
