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

//int 
//main(int argc, char **argv)
//{
//	users *users_start;
//	//users *cur;
//	int first = 1;
//
//	char *users_names[] = { "a", "b", "c", "b", "d" };
//	char *messag[] = { "hey a", "hey b", "hey c", "hey b", "hey d"};
//
//	/* First user*/
//	users_start = malloc(sizeof *users_start);
//	users_start->name = users_names[0];
//	users_start->next = NULL;
//	users_start->head = malloc(sizeof (messages));
//	users_start->head->msg = messag[0];
//	users_start->tail = users_start->head;
//	//cur = users_start;
//
//	int N = 5;
//	for (int i = 1; i < N; i++)
//	{
//		users *target = find_user(users_start, users_names[i]);
//		if (find_user(users_start, users_names[i]) == NULL)
//			add_user(users_start, users_names[i], messag[i]);
//		else
//			add_message(target, messag[i]);
//	}
//
//	users *target = find_user(users_start, "c");
//	delete_message(target);
//	add_message(target, messag[0]);
//
//	for (; users_start != NULL; users_start = users_start->next)
//	{
//		printf("Username: %s\n", users_start->name);
//		messages *mess = users_start->head;
//		for (; mess != NULL; mess = mess->next)
//			printf("Message: %s\n", mess->msg);
//	}
//	return 0;
//}
