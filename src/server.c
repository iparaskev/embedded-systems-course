#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define BACKLOG 10
#define MAXDATASIZE 100

void 
zombies_termination (int signum)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}

int 
main(int argc, char **argv)
{
	if (argc < 2){
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
	char buffer[MAXDATASIZE];

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
	int c;
	c = bind(listen_fd, (struct sockaddr *) &server_addr, sizeof server_addr);
	if ( c == -1)
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

	// reap zombie processes
	signal(SIGCHLD, zombies_termination);

	socklen_t addr_size = sizeof client_addr;
	int numbytes;
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

		if(!fork())
		{
			// after accepting the process doesn't need it
			close(listen_fd);
			
			if ((numbytes = recv(new_fd, buffer, MAXDATASIZE - 1, 0)) == -1)
			{
				perror("recv");
				exit(1);
			}

			buffer[numbytes] = '\0';
			printf("The message was: %s", buffer);

			close(new_fd);
			exit(0);
		}
		close(new_fd);
	}
	
	return 0;
}
