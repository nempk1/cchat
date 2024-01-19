#include "job_exec.h"
#include "parse_msg.h"

// Thread responsible for showing messages on and exec the cmd's.
void *print_exec_thread(void *arg) {
	
	job_args_t *args = (job_args_t*) arg;
	squeue *procqueue = args->q_proc_msg;

	struct message *proc_msg = NULL;

	while(1) {
		proc_msg = squeue_dequeue_data(procqueue);
		if(proc_msg != NULL) {
			switch(proc_msg->command) {
				case PRIVMSG:
					if (proc_msg->is_command) {
						cmd_interp(proc_msg);
					}
					if (proc_msg->rgb_8bit) {
						if(proc_msg->mod) {
							printf("\033[1;32m%s\033[1;0m \033[38;2;%u;%u;%um%s\033[1;0m:%s\n",
							"\U000024c2",
							proc_msg->rgb_8bit[0],
							proc_msg->rgb_8bit[1],
							proc_msg->rgb_8bit[2],
							proc_msg->username,
						       	proc_msg->msg);							
						} else {

						printf("\033[38;2;%u;%u;%um%s\033[1;0m:%s\n",
						proc_msg->rgb_8bit[0],
						proc_msg->rgb_8bit[1],
						proc_msg->rgb_8bit[2],
						proc_msg->username,
					       	proc_msg->msg);
						}
					}
					break;
				case JOIN:
					printf("\033[1;32mUSER:%s JOINED\033[1;0m\n",
						       	proc_msg->username);
					break;
				case PART:
					printf("\033[1;31mUSER:%s LEFT\033[1;0m\n",
							proc_msg->username);
					break;
				default:
					break;
			} 
			message_destroy(&proc_msg);
			continue;
		}
		/* Wait on proc msg semaphore */
		sem_wait(args->q_proc_sem);
	}
}


