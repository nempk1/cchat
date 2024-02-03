#include "commands.h"
#include "command_interp.h"

#include "irc_parse.h"
#include "message.h"

static struct command_str cmd_list[] = 
{
	{"!test", test_command, 0},
	{"!mem_usage", memory_usage, 0},
	{NULL, NULL, 0}
};

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

