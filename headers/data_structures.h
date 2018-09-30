#include <pthread.h>

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

/* Linked list for storing the unread messages of every user. */
typedef struct message
{
	char *msg;
	struct message *next;
} messages;

/* Linked list for users.
 *
 * name -- User id.
 * head -- Pointer to the start of the unread messages structure.
 * tail -- Pointer to the end of the unread messages structure.
 * user_mutex -- Unique key for every user for adding or deleting messages.
 * next -- Pointer to the next user.
*/
typedef struct user
{
	char *name;
	messages *head, *tail;
    pthread_mutex_t user_mutex;
	struct user *next;
} users;

/* Structure for the input arguments of every thread.*/
struct accept_d
{
	int new_fd;
};

/* Linked list for the waiting connections*/
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
