#include "commands.h"
#include "command_interp.h"

#include "parse_msg.h"

static struct command_str cmd_list[] = 
{
	{"!teste", test_command},
	{0, 0}
};

int cmd_interp(struct message *userinfo)
{
	const struct command_str *list = cmd_list;
	while(list->cmd_str != NULL) {
		if(!strcmp(list->cmd_str, userinfo->msg)) {
			list->fun_ptr(userinfo);
			return 0;
		}
		else
			list++;
	}
	return 0;
}

