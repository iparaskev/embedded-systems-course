#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "data_structures.h"

void 
add_message(users *list, char *message)
{
	messages *next;
	if ((next = malloc(sizeof *next)) == NULL)
	{
		perror("malloc: Add user");
		exit(1);
	}
	next->msg = message;
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

void
add_user(users *list, char *username, char *message)
{
	for (; list->next != NULL; list = list->next);
	/* Initialization of the new user.*/
	users *next;
	if ((next = malloc(sizeof *next)) == NULL)
	{
		perror("malloc: Add user");
		exit(1);
	}
	next->name = username;
	pthread_mutex_init(&(next->user_mutex), NULL);

	/* Initialize the first message.*/
	next->tail = NULL;
	add_message(next, message);
	next->next = NULL;

	/* Update the list.*/
	list->next = next;
	list = list->next;	
}

users*
find_user(users *list, char *username)
{
	users *current = list;
	for (; current != NULL; current = current->next)
		if (!strcmp(current->name, username))
			break;

	return current;
}


void
delete_message(users *user)
{
	if (user->head == user->tail)
	{
		free(user->head);
		user->head = NULL;
		user->tail = NULL;
	}
	else
	{
		messages *cur = user->head;
		user->head = user->head->next;
		free(cur);
	}
}

void 
add_descriptor(fifo *l, struct accept_d *sd)
{
	fifo *cur = l;
	for (; cur->next != NULL; cur = cur->next);

	fifo *new = malloc(sizeof *new);
	if (new == NULL)
	{
		perror("Malloc");
		exit(1);
	}
	new->sd = sd;
	new->next = NULL;
	cur->next = new;
}

void
delete_descriptor(fifo *l)
{
	fifo *del = l->next;
	l->sd = l->next->sd;
	l->next = l->next->next;
	free(del);	
}

//int 
//main(int argc, char **argv)
//{
//	fifo *test = NULL;
//	int N = 10;
//	test = malloc(sizeof *test);
//	test->sd = (void *) (long) 0;
//	for (long i = 1; i < N; i++)
//		add_descriptor(test, (void *) i);	
//
//	for (int i = 0; i < N-1; i++)
//		delete_descriptor(test);
//	for(; test != NULL; test = test->next)
//	{
//		printf("%ld\n", (long) test->sd);
//	}
//
//	return 0;
//}
