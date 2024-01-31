#ifndef _JOB_ARGS_H_
#define _JOB_ARGS_H_

#include <openssl/ssl.h>
#include <semaphore.h>
#include "config.h"
#include "queue.h"

typedef struct {
	// Queue used to store
	// msg's originated from
	// socket.
	squeue 	*q_raw_msg;
	sem_t 	*q_raw_sem;

	// Queue used to store
	// msg's processed from
	// the raw queue.
	squeue 	*q_proc_msg;
	sem_t  	*q_proc_sem;

	// Queue used to pass
	// jobs between threads.
	squeue 	*q_jobs;

	struct T_SSL* sockfd; // reference of connected socket.	
	struct p_cfg * cfg;
	int sockjava; // socket used for java spotify commands.
} job_args_t;

#endif
