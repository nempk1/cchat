#include "commands.h"
#include "command_interp.h"

#include "irc_parse.h"
#include "message.h"

static struct command_str cmd_list[] = 
{
	{"!teste", test_command},
	{"!mem_usage", memory_usage},
	{NULL, NULL}
};

int cmd_interp(struct twitch_msg *msg)
{
	const struct command_str *list = cmd_list;
	while(list->cmd_str != NULL) {
		if(strstr(msg->irc->param.last, list->cmd_str)) {
			list->fun_ptr(msg);
			return 0;
		}
		else
			list++;
	}
	return 0;
}

