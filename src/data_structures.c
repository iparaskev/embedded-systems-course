#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "data_structures.h"
#include "constants.h"

/* Add a new message to the messages list for a user.
 *
 * Arguments:
 * list -- Pointer to a user struct.
 * message -- The message to be added.
*/
void 
add_message(users *list, char *message)
{
	messages *next;
	if ((next = malloc(sizeof *next)) == NULL)
	{
		perror("malloc: Add user");
		exit(1);
	}

	/* Copy message to new location*/
	if ((next->msg = malloc(MAXDATA_SIZE)) == NULL)
	{
		perror("malloc: New message");
		exit(1);
	}

	strcpy(next->msg, message);
	next->next = NULL;

	/* Update the list.*/
	if (list->tail == NULL)
	{
		list->tail = next;
		list->head = next;
	}
	else
	{
		list->tail->next = next;
		list->tail = list->tail->next;
	}
}

/* Add a user to the users list and add the first message.
 *
 * Arguments:
 * list -- Pointer to the head of the users list.
 * username -- The id of the user.
 * message -- The first unread message. 
*/
void
add_user(users *list, char *username, char *message)
{
	/* Go to the last user.*/
	for (; list->next != NULL; list = list->next);

	/* Initialization of the new user.*/
	users *next;
	if ((next = malloc(sizeof *next)) == NULL)
	{
		perror("malloc: Add user");
		exit(1);
	}

	/* Copy the name to new memory location*/
	if ((next->name = malloc(ADDRESS_SIZE)) == NULL)
	{
		perror("malloc: Name user");
		exit(1);
	}
	strcpy(next->name, username);

	/* Initialize the mutex of the user*/
	pthread_mutex_init(&(next->user_mutex), NULL);

	/* Initialize the first message.*/
	next->tail = NULL;
	add_message(next, message);
	next->next = NULL;

	/* Update the list.*/
	list->next = next;
	list = list->next;	
}

/* Find a user with a specific username and return the pointer that points at 
 * him.
 *
 * Arguments:
 * list -- The users list.
 * username -- The username of the user we want
 *
 * Output:
 * A pointer to the specific user.
*/
users*
find_user(users *list, char *username)
{
	//users *current = list;
	for (; list != NULL; list = list->next)
		if (!strcmp(list->name, username))
			break;

	return list;
}


/* Delete the first message from a user's message list.*/
void
delete_message(users *user)
{
	/* If the list has only one message.*/
	if (user->head == user->tail)
	{
		free(user->head->msg);
		free(user->head);
		user->head = NULL;
		user->tail = NULL;
	}
	else
	{
		messages *cur = user->head;
		user->head = user->head->next;
		free(cur->msg);
		free(cur);
	}
}

/* Add a new open socket descriptor to the waiting quee.*/
void 
add_descriptor(fifo *l, struct accept_d *sd)
{
	/* Go to the end of the quee.*/
	for (; l->next != NULL; l = l->next);

	/* Copy the new descriptor*/
	fifo *new = malloc(sizeof *new);
	if (new == NULL)
	{
		perror("Malloc");
		exit(1);
	}

	/* Update the quee.*/
	new->sd = sd;
	new->next = NULL;
	l->next = new;
}

/* Delete an element from the waiting quee*/
void
delete_descriptor(fifo *l)
{
	fifo *del = l->next;
	l->sd = l->next->sd;
	l->next = l->next->next;
	free(del);	
}
