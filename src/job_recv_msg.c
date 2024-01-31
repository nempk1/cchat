#include "job_recv_msg.h"
#include <string.h>


#define PING_SEND "PING :tmi.twitch.tv"
#define PING_RESPONSE "PONG :tmi.twitch.tv\r\n"

void *job_recv_msg (void *args)
{
	job_args_t *arg = (job_args_t *) args;

	while(1) {
		char *msg = calloc(IRC_MAX_TAGS_MESSAGE, sizeof(char));

		int tmp = T_SSL_read(arg->sockfd, msg, 
				IRC_MAX_TAGS_MESSAGE - 1);

		/* Make sure is null terminated string */
		msg[IRC_MAX_TAGS_MESSAGE-1] = '\0';
		
		/* Write PONG as response to PING. */
		if(tmp == 21) {
			T_SSL_write(arg->sockfd, PING_RESPONSE, strlen(PING_RESPONSE));
			free(msg);
			continue;	
		} else if (tmp == 0) {
			fprintf(stderr, "Connection closed.\n");
			exit(0);	
		} else if (tmp == -1) {
			free(msg);
			continue;
		} else if (tmp < 0 ) {
			fprintf(stderr, "Error on T_SSL_read call %s %d\n", __FILE__, __LINE__);
			exit(0);
		} else {
			int j = 0;
			/*
			char *tmp_ptr = msg;
			*/
			for(int i = 0; i < tmp; i++) {
				if(msg[i] == '\n')
					j++;
			}
			/*
			do {
				tmp_ptr = strchr(tmp_ptr, '\n');
				j++; tmp_ptr++;
			}while (tmp_ptr != NULL);
			*/
			char *msg_ptr = msg;
			if(j > 1) {
				while(--j != 0) {
					char *tmp = strsep(&msg_ptr, "\n");
					squeue_enqueue_data(arg->q_raw_msg, strdup(tmp));
					sem_post(arg->q_raw_sem);
				}
				free(msg);
				continue;
			}
			_rtrim(msg);
			squeue_enqueue_data(arg->q_raw_msg, msg);
			/* Post raw msg semaphore */
			sem_post(arg->q_raw_sem);
		}
	}
}
