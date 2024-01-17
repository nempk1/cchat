#ifndef _PROC_RECV_
#define _PROC_RECV_

#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "queue.h"
#include "parse_msg.h"

extern void *_job_proc_recv(void *);
#endif
