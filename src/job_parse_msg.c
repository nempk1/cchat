#include "job_args.h"
#include "job_parse_msg.h"
#include "basic.h"
#include "message.h"
#include "parse_msg.h"

void *_job_proc_recv(void *arg)
{
	job_args_t *args =  (job_args_t*) arg;
	struct timespec ts;
	ts.tv_sec = 100 / 1000;
	ts.tv_nsec = (100 % 1000) * 1000000;
	
	while(1) {
		char *raw_msg = squeue_dequeue_data(args->q_raw_msg);
		
		if(raw_msg == NULL) {
			nanosleep(&ts, &ts);
			continue;
		}
		
		struct message* result= message_create(raw_msg);

		if (parse_message(raw_msg, result)) {
			message_destroy(&result);
		} else {
			result->cfg = args->cfg;
			squeue_enqueue_data(args->q_proc_msg, result);
		}
	}
}
