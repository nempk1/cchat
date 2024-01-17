#include "twitch_util.h"

int respond_message(struct T_SSL *conn,
			struct message *msg,
			const char *response)
{
	int ret = 0;
	char *msg_id = search_tag_name("id", msg->tag_list);
	if(!msg_id) {
		return -1;
	}

	char *result_str = NULL;	
	ret = asprintf(&result_str, "@reply-parent-msg-id=%s PRIVMSG #%s :%s\n",
		       	msg_id, msg->cfg->channel, response);
	if(ret == -1) {
		return -1;
	}

	ret = T_SSL_write(conn, result_str, strlen(result_str));
	if(ret <= 0) {
		return -1;
	}
	
	free(result_str);
	return 0;
}

