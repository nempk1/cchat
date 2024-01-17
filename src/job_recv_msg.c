#include "job_recv_msg.h"


#define PING_SEND "PING :tmi.twitch.tv"
#define PING_RESPONSE "PONG :tmi.twitch.tv\r\n"

void *_job_recv_f (void *args)
{
	job_args_t *arg = (job_args_t *) args;

	while(1) {
		char *msg = calloc(IRC_MAX_TAGS_MESSAGE, sizeof(char));

		int tmp = T_SSL_read(arg->sockfd, msg, 
				IRC_MAX_TAGS_MESSAGE - 1);
		
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
			squeue_enqueue_data(arg->q_raw_msg, msg);
			
		}
	}
}
