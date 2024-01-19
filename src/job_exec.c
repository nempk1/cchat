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
			if (proc_msg->command == PRIVMSG) {
				printf("%s: %s\n",
					proc_msg->username, proc_msg->msg);
			} 
			if (proc_msg->is_command) {
				cmd_interp(proc_msg);
			}
			message_destroy(&proc_msg);
			continue;
		}
		/* Wait on proc msg semaphore */
		sem_wait(args->q_proc_sem);
	}
}


