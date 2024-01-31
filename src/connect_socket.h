#ifndef _CONNECT_SOCKET_
#define _CONNECT_SOCKET_

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* Structure used to share SSL obj between thread's. */

struct T_SSL {
	SSL *ssl;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	bool want_to_write;
};

extern int
T_SSL_init(struct T_SSL**);

extern void
T_SSL_free(struct T_SSL**);

extern struct T_SSL*
T_SSL_connect(char *, char *);

/***************************************
 * wrapper for SSL_write() function using 
 * pthreads lock and cond 
 * to avoid problems.
 *
 ***************************************/

extern int
T_SSL_write(struct T_SSL *, const void *, int);

/***************************************
 * wrapper for SSL_read() function using 
 * pthreads lock and cond 
 * to avoid problems.
 *
 ***************************************/

extern int 
T_SSL_read(struct T_SSL *, void *, int);

/***************************************
 * This function creates a connected 
 * socket according to the args
 * 
 * arg 1: string with the ip address
 * 	  or domain name
 * 
 * arg 2: string with the port number
 *
 * returns: -1 on error
 * 	    or connected socket fd
 *
 ***************************************/

extern int connect_socket(char *ip, char *port);

/***************************************
 * This function just envolves the orig 
 * send func for one with mutex lock for 
 * using on the same file descriptor
 *
 ***************************************/

extern ssize_t CUSTOM_send(int fd, const void *buf, size_t len, int flags);

#endif
