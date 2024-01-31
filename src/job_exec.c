#include "job_exec.h"
#include "irc_parse.h"
#include "message.h"

#include "assert.h"
			/* Unicode emoji in the midle */
#define MOD_TAG  	"\033[1;32m\U000024c2\033[1;0m "

#define UNAME_RGB 	"\033[38;2;%u;%u;%um%s\033[1;0m"



// Thread responsible for showing messages on and exec the cmd's.
void *
job_exec_thread(void *arg) 
{
	job_args_t *args = (job_args_t*) arg;
	squeue *procqueue = args->q_proc_msg;

	while(1) {
		struct twitch_msg *msg	= squeue_dequeue_data(procqueue);
		if(msg != NULL) {
			switch(msg->cmd) {
				case PRIVMSG:
					if (msg->is_command) {
						cmd_interp(msg);
					}
					if(msg->mod) {
						printf(MOD_TAG UNAME_RGB":%s\n",
								msg->rgb[0],
								msg->rgb[1],
								msg->rgb[2],
								msg->username,
								msg->irc->param.last);
						break;
					} 
					printf(UNAME_RGB":%s\n",
							msg->rgb[0],
							msg->rgb[1],
							msg->rgb[2],
							msg->username,
							msg->irc->param.last);

					break;
				case JOIN:
					assert(msg->irc != NULL);
					printf("\033[1;32mUSER:%s JOINED\033[1;0m\n",
							msg->irc->source.name);
					break;
				case PART:
					assert(msg->irc != NULL);
					printf("\033[1;31mUSER:%s LEFT\033[1;0m\n",
							msg->irc->source.name);
					break;
				default:
					break;
			} 
			t_msg_free(&msg);
			continue;
		}
		/* Wait on proc msg semaphore */
		sem_wait(args->q_proc_sem);
	}
}


