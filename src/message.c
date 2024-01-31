#include "message.h"
#include "irc_parse.h"

struct twitch_msg *
t_msg_alloc() 
{
	struct twitch_msg *res = malloc(sizeof(*res));
	if(!res) {
		return NULL;
	}
	res->cfg 	= NULL;
	res->irc 	= NULL;
	res->rgb 	= NULL;
	res->username 	= NULL;
	res->conn 	= NULL;
	res->cmd 	= NO_ID;

	return (res);
}

void t_msg_free(struct twitch_msg **msg)
{
	irc_msg_free((*msg)->irc);
	free((*msg)->rgb);
	free((*msg));
	(*msg) = NULL;
}
