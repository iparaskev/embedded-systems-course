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

struct accept_d
{
	int new_fd;
};

typedef struct fifos
{
        struct accept_d *sd;
        struct fifos *next;
} fifo;


void add_message(users *list, char *message);
void add_user(users *list, char *username, char *message);
users *find_user(users *list, char *username);
void delete_message(users *user);
void add_descriptor(fifo *l, struct accept_d *sd);
void delete_descriptor(fifo *l);

#endif
