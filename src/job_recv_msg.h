#ifndef _JOB_RECV_
#define _JOB_RECV_

#include <stdio.h>

#include "queue.h"
#include "message.h"
#include "job_args.h"
#include "basic.h"
#include "connect_socket.h"

/*****************************************
 * This function is the thread responsible
 * for receiving msg's from the socket and
 * storing in the raw queue.
 * 
 * arg 1: need to be casted inside to
 * 	  job_args_t 
 *
 ****************************************/

extern void *_job_recv_f(void * args);

#endif
