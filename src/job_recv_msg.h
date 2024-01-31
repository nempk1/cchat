#ifndef _JOB_RECV_
#define _JOB_RECV_

#include <stdio.h>

#include "queue.h"
#include "message.h"
#include "job_args.h"
#include "connect_socket.h"
#include "irc_parse.h"

/*****************************************
 * This function is the thread responsible
 * for receiving msg's from the socket and
 * storing in the raw queue.
 * 
 * arg 1: need to be casted inside to
 * 	  job_args_t 
 *
 ****************************************/

extern void *job_recv_msg(void * args);

#endif
