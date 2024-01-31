#include "irc_parse.h"
#include "job_args.h"
#include "job_parse_msg.h"
#include "message.h"
#include <assert.h>
#include <stdlib.h>

// copied from http://www.cse.yorku.ca/~oz/hash.html
static unsigned long _hash(unsigned char *str)
{
	unsigned int hash = 5381;
        int c;

        while ((c = *str++))
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

// copied from https://gist.github.com/xsleonard/7341172
// this is slight modified
static unsigned char* hexstr_to_char(const char* hexstr)
{
    unsigned char* chrs = malloc(6 * sizeof(*chrs));
    if(!chrs) {
    	fputs("Error allocating memory for chrs in hexstr_to_char\n", stderr);
	exit(EXIT_FAILURE);
    }
    for (size_t i=0, j=0; j<6; i+=2, j++)
        chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i+1] % 32 + 9) % 25;
    return chrs;
}

const struct cmd_hash command_hash[] = 
{
	{CLEARMSG , 0xD0F27FF3},
	{GLOBALUSERSTATE, 0x7134EAF6},
	{HOSTTARGET, 0xEF39400A},
	{NOTICE, 0xC39644E7},
	{PRIVMSG, 0xD693152D},
	{JOIN, 0x7C86FD55},
	{PART, 0x7C8A0D3C},
	//{PING, 0x7C8A2EB3,},
	{CAP, 0xB87D899},
	{RECONNECT, 0xD32BF926},
	{ROOMSTATE, 0xA52C87A3,},
	{USERNOTICE, 0x6325C926,},
	{WHISPER, 0xDA158E07},
	{NO_ID, 0xDAF672},
	{ 0, 0 },
};
void *_job_proc_recv(void *arg)
{


	job_args_t *args =  (job_args_t*) arg;
	const struct cmd_hash *table = NULL;
	
	while(1) {
		char *raw_msg = squeue_dequeue_data(args->q_raw_msg);
		table = NULL;
		
		/* Semaphore waits for arrivel of 
		 * more raw messages in queue.
		 */
		if(raw_msg == NULL) {
			sem_wait(args->q_raw_sem);
			continue;
		}

		char *color_hex = NULL;
		char *mod_tag = NULL;
		table = command_hash;

		
		struct twitch_msg* result = t_msg_alloc();
		result->irc = irc_msg_alloc();

		if(irc_parse(result->irc, raw_msg)) {
			fputs("Error trying to parse message received NULL struct messsage\n", stderr);
			exit(0);
		}
		result->cfg = args->cfg;
		result->conn = args->sockfd;

		unsigned long hash_result = _hash(
			(unsigned char *)result->irc->param.params[0]);

		while(table->hash_value != 0 )
		{	
			if(table->hash_value == hash_result) {
				result->cmd = table->cmd;			
				break;
			}
			table++;
		}

		switch(result->cmd) {
			case PRIVMSG:
				result->is_command = *result->irc->param.last == '!' ? 1 : 0;

				mod_tag = search_tag_name("mod", &result->irc->tlist);

				if(mod_tag) {
					result->mod = strtol(mod_tag, NULL, 10);
				}
				result->username = search_tag_name("display-name", &result->irc->tlist);

				color_hex =  search_tag_name("color", &result->irc->tlist);
				/* Not found */
				if(!color_hex) 
					break;

				/* Walk forward 1 char 
				 * because it starts with #
				 */
				color_hex++;
				result->rgb = hexstr_to_char(color_hex);

				break;
			default:
				break;
		}

		squeue_enqueue_data(args->q_proc_msg, result);
		/* Post semaphore for proc msg queue */
		sem_post(args->q_proc_sem);
	}
}
