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
					printf("%s: %s\n",
						proc_msg->username,
					       	proc_msg->msg);
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


