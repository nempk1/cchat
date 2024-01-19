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


		unsigned char *color = NULL;
		char *color_hex = NULL;
		char *mod_tag = NULL;
		
		struct message* result= message_create(raw_msg);

		if (parse_message(raw_msg, result)) {
			message_destroy(&result);
		} else {
			result->cfg = args->cfg;
			result->conn = args->sockfd;
			switch(result->command) {
				case PRIVMSG:
					mod_tag = search_tag_name("mod", result->tag_list);

					if(mod_tag) {
						result->mod = strtol(mod_tag, NULL, 10);
					}

					color_hex =  search_tag_name("color", result->tag_list);
					/* Not found */
					if(!color_hex) 
						break;
					/* Walk forward 1 char 
					 * because it starts with #
					 */
					color_hex++;
					color = hexstr_to_char(color_hex);
					/*
					color[0] = (char)((color[0] *6 /256)*36);
					color[1] = (char)((color[1] *6 /256)*6);
					color[2] = (char)((color[2] *6 /256));
					*/
					result->rgb_8bit = color;
				default:
					break;
			}
			squeue_enqueue_data(args->q_proc_msg, result);
			/* Post semaphore for proc msg queue */
			sem_post(args->q_proc_sem);
		}
	}
}
