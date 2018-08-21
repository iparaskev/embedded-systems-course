#include <pthread.h>

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

typedef struct message
{
	char *msg;
	struct message *next;
} messages;

typedef struct user
{
	char *name;
	messages *head, *tail;
    pthread_mutex_t user_mutex;
	struct user *next;
} users;

void add_message(users *list, char *message);
void add_user(users *list, char *username, char *message);
users *find_user(users *list, char *username);
void delete_message(users *user);

#endif
