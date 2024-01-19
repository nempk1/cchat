#include "job_args.h"
#include "job_parse_msg.h"
#include "basic.h"
#include "message.h"
#include "parse_msg.h"

void *_job_proc_recv(void *arg)
{
	job_args_t *args =  (job_args_t*) arg;
	
	while(1) {
		char *raw_msg = squeue_dequeue_data(args->q_raw_msg);
		
		/* Semaphore waits for arrivel of 
		 * more raw messages in queue.
		 */
		if(raw_msg == NULL) {
			sem_wait(args->q_raw_sem);
			continue;
		}
		
		struct message* result= message_create(raw_msg);

		if (parse_message(raw_msg, result)) {
			message_destroy(&result);
		} else {
			result->cfg = args->cfg;
			result->conn = args->sockfd;
			squeue_enqueue_data(args->q_proc_msg, result);
			/* Post semaphore for proc msg queue */
			sem_post(args->q_proc_sem);
		}
	}
}
