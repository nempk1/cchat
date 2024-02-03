#include <string.h>
#include "commands.h"
#include "command_interp.h"
#include "irc_parse.h"
#include "message.h"
#include "strlcat.h"

static void*show_commands(void*);

static struct command_str cmd_list[] = 
{
/* 	You can use the same command function
 *  	with another cmd name as an 'alias'
 *  	if place it one after the other you 
 *  	get the (!cmd !cmd_alias) response
 *  	from show_commands
 */
	{"!mem_usage", memory_usage, 0},
	{"!commands", show_commands, 0},
	{NULL, NULL, 0}
};

static void *show_commands(void *data)
{

	struct twitch_msg *msg = data;
	const struct command_str *list = cmd_list;
	
	#define SIZE_B 512
	char buffer[SIZE_B];
	size_t end;
	buffer[0] = '\0';
	while(list->cmd_str != NULL) {
		if((list + 1)->fun_ptr == list->fun_ptr) {
			strlcat(buffer, "(", SIZE_B);
			strlcat(buffer+ strlcat(buffer, list->cmd_str, SIZE_B), " = ", SIZE_B);
			do {
				list++;
				strlcat(buffer + (end = strlcat(buffer, list->cmd_str, SIZE_B)) , " = ", SIZE_B);
			} while ((list + 1)->fun_ptr == list->fun_ptr);
			buffer[end] = '\0';
			strlcat(buffer, ") ", SIZE_B);
		}else 
			strlcat(buffer + strlcat(buffer, list->cmd_str, SIZE_B), " ", SIZE_B);

		list++;
	}

	respond_message(msg->conn,msg,buffer);
	return NULL;
}

int cmd_interp(struct twitch_msg *msg)
{
	const struct command_str *list = cmd_list;
	char *cmd = msg->irc->param.last, *res = NULL;
	cmd = _trim(cmd);
	res = strsep(&cmd, " ");
	while(list->cmd_str != NULL) {
		if(!strcmp(res, list->cmd_str)) {
			if(list->has_arg) {
				struct cmd_arg arg= {
					msg,
					cmd
				};
				list->fun_ptr(&arg);
			} else
				list->fun_ptr(msg);
			return 0;
		}
		else
			list++;
	}
	respond_message(msg->conn,msg,"Invalid command in CCHAT!!!");
	return 0;
}

