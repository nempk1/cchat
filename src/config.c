#include "config.h"
#include "connect_socket.h"
#include "simple_config.h"

int
config_send(struct T_SSL *conn, const char *filepath, const char *channel_name)
{
	struct s_config *cfg = config_new();
	const char *oauth, *oauth_name, *channel_chat;
	int enable_tag = true;
	int enable_command = true;
	int enable_membership = true;


	/* Read the file. 
	 * If there is an error, report it and exit. */
  	config_parse_file(filepath, cfg);
	oauth = config_get_string("oauth", cfg);
	oauth_name = config_get_string("oauth_nick", cfg);
	channel_chat = config_get_string("channel", cfg);

	char *result[3] = { NULL };
	asprintf(&result[0], "%s%s\r\n", OAUTH_PREFIX, oauth);
	if(result[0] == NULL)
		return -1;

	asprintf(&result[1], "%s%s\r\n", NICK_PREFIX, oauth_name);
	if(result[1] == NULL)
		return -1;

	if(channel_name)
		channel_chat = channel_name;

	asprintf(&result[2], "%s%s\r\n", JOIN_PREFIX, channel_chat);
	if(result[2] == NULL)
		return -1;

	
	
	for(int i = 0; i < 3; i++)
		if(T_SSL_write(conn, result[i], strlen(result[i])) <= 0) 
			return -1;
	if(enable_command && enable_membership && enable_tag) {
		char buffer[100];
		T_SSL_write(conn, ENABLE_ALL, sizeof(ENABLE_ALL)-1);
		T_SSL_read(conn,buffer, sizeof(buffer));
	}
	
	config_destroy(&cfg);
	for(int i = 0; i< 3; i++)
		free(result[i]);	

	return 0;
}

struct p_cfg*
pconfig_parse_file(const char *filepath) 
{
	struct s_config *cfg = config_new();
	config_parse_file(filepath, cfg);
	struct p_cfg *result = malloc(sizeof(*result));

	result->oauth_nick = strdup(config_get_string("oauth_nick", cfg));
	result->channel = strdup(config_get_string("channel",cfg));
	result->commands = true;
	result->tags = true;
	result->oauth = strdup(config_get_string("oauth", cfg));

	if(result->oauth == NULL ||
		result->oauth_nick == NULL ||
		result->channel == NULL )
	{
		free(result->oauth_nick);
		free(result->channel);
		free(result->oauth);
		free(result);
		return NULL;		
	}

	config_destroy(&cfg);
	return result;
}
