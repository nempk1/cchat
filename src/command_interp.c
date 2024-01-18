#include "command_interp.h"


int cmd_interp(struct message *userinfo, struct T_SSL* SOCKFD, int sockjava)
{

	if ( (strncmp(userinfo->msg, "!musica", 7) == 0) &&
			(strlen(userinfo->msg) == 7)) {
		respond_message(SOCKFD, userinfo, "Fake music command working");
		return 0;	
	}
	return 0;
}

