#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include "command_interp.h"
#include "connect_socket.h"
#include "queue.h"
#include "config.h"
#include "job_recv_msg.h"
#include "job_exec.h"
#include "job_parse_msg.h"

int send_cfg(struct T_SSL* sockfd);

int main(int argc, char *argv[])
{

	char *irc_address 	= NULL;
	char *irc_port 		= NULL;
	char *config_file 	= NULL;
	char *channel_name 	= NULL;

	struct p_cfg *cfg 	= NULL;

	enum {
		PORT_OPT,
		ADDR_OPT,
	};

	struct option longopts[] = {
		{ "config-file",	required_argument,	NULL,	'c' },	
		{ "irc",		required_argument,	NULL,   'i' },
		{ "irc-port",		required_argument, 	NULL,	'p' },
		{ "channel", 		required_argument, 	NULL, 	'n' },
		{ NULL,			0,			NULL,		0}
	};


	int ch = 0;
	while((ch =getopt_long(argc,argv, "nc:i:p:", longopts, NULL)) != -1)
	{
		switch(ch) {
			case 'c':
				config_file = strdup(optarg);
				break;
			case 'i':
				irc_address = strdup(optarg);
				break;
			case 'p':
				irc_port = strdup(optarg);
				break;
			case 'n':
				channel_name = strdup(optarg);
				break;
			case '?': 
				exit(EXIT_FAILURE);
			default:
				break;
				
		}
	
	}

	struct T_SSL* sockfdorg = T_SSL_connect(irc_address, irc_port);
	
	if (sockfdorg == NULL) {
		fprintf(stderr, "Error trying to connect socket with: %s %s\n", 
					irc_address, irc_port); 
		exit(EXIT_FAILURE);
	}

	free(irc_port);
	free(irc_address);

	cfg = config_parse_file(config_file);
	if(!cfg) {
		printf("Error parsing config file %s\n", config_file);
		exit(EXIT_FAILURE);	
	}

	if(channel_name) {
		free(cfg->channel);
		cfg->channel = channel_name;
	}

	if(config_send(sockfdorg, config_file, channel_name)) {
		printf("Error sending cfg\n");
		exit(EXIT_FAILURE);
	}

	free(config_file);

	int sockfd_java = -1;


	struct squeue *q_raw_msg = NULL;
	struct squeue *q_proc_msg = NULL;

	// Create queue's needed
	squeue_init(&q_raw_msg);
	squeue_init(&q_proc_msg);

	sem_t parsed_messages;
	sem_init(&parsed_messages, 0, 0);
	sem_t raw_messages;
	sem_init(&raw_messages, 0, 0);


	job_args_t *job_args = malloc(sizeof(job_args_t));	
	if(job_args == NULL) 
	{
		printf("error on job_args allocation");
		exit(EXIT_FAILURE);
	}
	
	job_args->q_raw_msg = q_raw_msg;
	job_args->q_raw_sem = &raw_messages;
	
	job_args->q_proc_msg = q_proc_msg;
	job_args->q_proc_sem = &parsed_messages;
	job_args->sockfd = sockfdorg;
	job_args->sockjava = sockfd_java;
	job_args->cfg = cfg;
	
	pthread_t cmd_thread;
	pthread_t recv_thread;
	pthread_t proc_msg_thread;

	
	if(pthread_create(&recv_thread, NULL, job_recv_msg, (void *)job_args)) {
		fprintf(stderr, "Error creating recv thread\n");
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&cmd_thread, NULL, job_exec_thread, (void *)job_args)) {
		fprintf(stderr, "Error creating print thread\n");
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&proc_msg_thread, NULL, _job_proc_recv, (void *)job_args)) {
		fprintf(stderr, "Error creating parse thread\n");
		exit(EXIT_FAILURE);
	}
	

	// Read data from stdio and
	// send to socket.
	char * str = NULL;
	char * sent = NULL;
	size_t len = 0;
	while (1) {
		int send_bytes = 0;
		int num = getline(&str, &len, stdin);
		if (num == -1) {
			perror("Error on read");
			exit(0);
		} else {
			len = num;
			if(!strcmp("exit\n", str)) {
				break; /* exit loop and go to exit */
			}
			asprintf(&sent, "PRIVMSG #%s :%s\r\n", cfg->channel,str);
			send_bytes = T_SSL_write(sockfdorg, sent, strlen(sent));
			if (send_bytes == -1) {
				perror("Error on send");
				exit(EXIT_FAILURE);	
			}
			free(sent);
		}
	}
	pthread_cancel(cmd_thread);
	pthread_cancel(recv_thread);
	pthread_cancel(proc_msg_thread);
	
	pthread_join(cmd_thread,NULL);
	pthread_join(recv_thread, NULL);
	pthread_join(proc_msg_thread, NULL);
	
	squeue_clear_all(&q_raw_msg);
	squeue_clear_all(&q_proc_msg);

	squeue_destroy(&q_raw_msg);
	squeue_destroy(&q_proc_msg);

	sem_destroy(&raw_messages);
	sem_destroy(&parsed_messages);
	T_SSL_free(&sockfdorg);
	exit(0);
}
