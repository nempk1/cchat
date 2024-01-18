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

	int enable_spotify = 0;

	char *suboptions	= NULL;
	char *irc_address 	= NULL;
	char *irc_port 		= NULL;
	char *config_file 	= NULL;
	char *spotify_port 	= NULL;
	char *spotify_addr 	= NULL;
	char *suboptvalue 	= NULL;

	struct parsed_cfg *cfg 	= NULL;;

	enum {
		PORT_OPT,
		ADDR_OPT		
	};

	char *const token[] = {
		[PORT_OPT] = "port",
		[ADDR_OPT] = "address",
		NULL	
	};

	struct option longopts[] = {
	{ "config-file",	required_argument,	NULL,	'c' },	
	{ "irc",		required_argument,	NULL,   'i' },
	{ "irc-port",		required_argument, 	NULL,	'p' },
	{ "enable-spotify",	required_argument,	&enable_spotify, 1},
	{ NULL,			0,			NULL,		0}
	};


	int ch = 0;
	while((ch =getopt_long(argc,argv, "c:i:p:", longopts, NULL)) != -1)
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
			case '?': 
				exit(EXIT_FAILURE);
				break;
			case 0:
				if(enable_spotify) {
				suboptions = optarg;
				while(*suboptions) {
					switch(getsubopt(&suboptions, token, &suboptvalue)) {
						case PORT_OPT:
							spotify_port = strdup(suboptvalue);
							break;
						case ADDR_OPT:
							spotify_addr = strdup(suboptvalue);
							break;
						default :
							break;
					}

				}
				}
			default:
				break;
				
		}
	
	}

	char buffer[4096] = {'\0'};

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

	if(config_send(sockfdorg, config_file)) {
		printf("Error sending cfg\n");
		exit(EXIT_FAILURE);
	}

	free(config_file);

	int sockfd_java = -1;
	if(enable_spotify) {
		if(enable_spotify != 0 && (spotify_addr == NULL || spotify_port == NULL)) {
			fprintf(stderr, "Erro with values of address or port!!\n");
		} else {	
			sockfd_java = connect_socket(spotify_addr, spotify_port);

			if (sockfd_java == -1) {
				fprintf(stderr, "Error trying to connecto to sptofiy-controller %s %s\n",spotify_addr,spotify_port);
				exit(EXIT_FAILURE);
			}
		}
	}

	free(spotify_addr);
	free(spotify_port);

	squeue *q_raw_msg = NULL;
	squeue *q_proc_msg = NULL;

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

	
	if(pthread_create(&recv_thread, NULL, _job_recv_f, (void *)job_args)) {
		fprintf(stderr, "Error creating recv thread\n");
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&cmd_thread, NULL, print_exec_thread, (void *)job_args)) {
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
			asprintf(&sent, "PRIVMSG #%s :%s\n", cfg->channel,str);
			send_bytes = T_SSL_write(sockfdorg, sent, strlen(sent));
			if (send_bytes == -1) {
				perror("Error on send");
				exit(EXIT_FAILURE);	
			}
			free(str);
			free(sent);
			str = NULL; len = 0;
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


	exit(1);
}
