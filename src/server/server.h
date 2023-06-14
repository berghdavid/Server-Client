#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include "queue.h"

typedef struct info info;

/** Stores all necessary data for a pthread. */
struct info
{
	int	id;
	int	server_socket;
	queue*	q;
};

/**
 * Sorts and returns an array using insertion sort. 
 * Insertion sort is fast and easy for small arrays.
 */
int* sort_array();

/** Prints the received/sent array of ints.
 * 
 * @param s String which is either "received" or "sent".
 * @param thr_id Int ID of thread.
 * @param buf Array of ints to be printed.
*/
void print_buf(char s[], int thr_id, int buf[]);

int create_socket();

/** 
 * Waits for a client to connect to, then communicates
 * with the connected client.
 */
void* wait_client(void* socket_desc);

/**
 * Reads user input and toggles quit to true if 'exit' is entered.
 * This joins all the threads and shutdowns the server.
 */
void* shutdown_server(void* quit);

int main();

#endif
