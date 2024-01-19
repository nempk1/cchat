#include "config.h"
#include "connect_socket.h"

int
config_send(struct T_SSL *conn, const char *filepath)
{
	config_t cfg;
	const char *oauth, *oauth_name, *channel_chat;
	int enable_tag = true;
	int enable_command = true;
	int enable_membership = true;

	config_init(&cfg);

	/* Read the file. 
	 * If there is an error, report it and exit. */
  	if(!config_read_file(&cfg, filepath)) {
    		fprintf(stderr, "%s:%d - %s\n",
				config_error_file(&cfg),
				config_error_line(&cfg),
			       	config_error_text(&cfg));

    		config_destroy(&cfg);
	    return -1;
  	}

	if(!config_lookup_string(&cfg, "oauth", &oauth)) {
		fprintf(stderr, "No 'oauth' setting in config file %s !!\n",filepath); 		
		config_destroy(&cfg);
	    	return -1;
	} 
	if(!config_lookup_string(&cfg, "oauth_nick", &oauth_name)) {
		fprintf(stderr, "No 'oauth_nick' setting in config file %s !!\n",filepath); 		
		config_destroy(&cfg);
	    	return -1;
	} 

	if(!config_lookup_string(&cfg, "channel", &channel_chat)) {
		fprintf(stderr, "No 'channel' setting in config file %s !!\n",filepath); 		
		config_destroy(&cfg);
	    	return -1;
	} 


	if(!config_lookup_bool(&cfg, "tags", &enable_tag)) {
		fprintf(stderr, "No 'tags' setting in config file %s !!\n",filepath); 		
		config_destroy(&cfg);
	    	return -1;
	} 

	if(strcmp(oauth, "") == 0) {
		fprintf(stderr, "No value set to 'oauth' in config file %s !!\n", filepath);	
		config_destroy(&cfg);
	    	return -1;
	} 
	
	if(strcmp(oauth_name, "") == 0) {
		fprintf(stderr, "No value set to 'oauth_nick' in config file %s !!\n", filepath);	
		config_destroy(&cfg);
	    	return -1;
	} 

	if(strcmp(channel_chat, "") == 0) {
		fprintf(stderr, "No value set to 'channel' in config file %s !!\n", filepath);	
		config_destroy(&cfg);
	    	return -1;
	} 

	char *result[3] = { NULL };
	asprintf(&result[0], "%s%s\r\n", OAUTH_PREFIX, oauth);
	if(result[0] == NULL)
		return -1;

	asprintf(&result[1], "%s%s\r\n", NICK_PREFIX, oauth_name);
	if(result[1] == NULL)
		return -1;

	asprintf(&result[2], "%s%s\r\n", JOIN_PREFIX, channel_chat);
	if(result[2] == NULL)
		return -1;

	
	for(int i = 0; i < 3; i++)
		if(T_SSL_write(conn, result[i], strlen(result[i])) <= 0) 
			return -1;
	if(enable_command && enable_membership && enable_tag) {
		char buffer[100];
		int result = 0;
		T_SSL_write(conn, ENABLE_ALL, sizeof(ENABLE_ALL)-1);
		result = T_SSL_read(conn,buffer, sizeof(buffer));
	}
	
	config_destroy(&cfg);
	for(int i = 0; i< 3; i++)
		free(result[i]);	

	return 0;
}

struct parsed_cfg *
config_parse_file(const char *filepath)
{
	config_t cfg;

	const char *oauth, *oauth_name, *channel_chat;
	int enable_tag = true;
	int enable_command = true;

	config_init(&cfg);

	/* Read the file. 
	 * If there is an error, report it and exit. */
  	if(!config_read_file(&cfg, filepath)) {
    		fprintf(stderr, "%s:%d - %s\n",
				config_error_file(&cfg),
				config_error_line(&cfg),
			       	config_error_text(&cfg));

    		config_destroy(&cfg);
		return NULL;
  	}

	if(!config_lookup_string(&cfg, "oauth", &oauth)) {
		fprintf(stderr, "No 'oauth' setting in config file %s !!\n",filepath); 		
		config_destroy(&cfg);
	    	return NULL;
	} 
	if(!config_lookup_string(&cfg, "oauth_nick", &oauth_name)) {
		fprintf(stderr, "No 'oauth_name' setting in config file %s !!\n",filepath); 		
		config_destroy(&cfg);
	    	return NULL;
	} 

	if(!config_lookup_string(&cfg, "channel", &channel_chat)) {
		fprintf(stderr, "No 'channel' setting in config file %s !!\n",filepath); 		
		config_destroy(&cfg);
	    	return NULL;
	} 


	if(!config_lookup_bool(&cfg, "tags", &enable_tag)) {
		fprintf(stderr, "No 'enable_tag' setting in config file %s !!\n",filepath); 		
		config_destroy(&cfg);
	   	return NULL;
	} 
	if(!config_lookup_bool(&cfg, "commands", &enable_command)) {
		fprintf(stderr, "No 'enable_command' setting in config file %s !!\n",filepath); 		
		config_destroy(&cfg);
	   	return NULL;
	}
	if(strcmp(oauth, "") == 0) {
		fprintf(stderr, "No value set to 'oauth' in config file %s !!\n", filepath);	
		config_destroy(&cfg);
	    	return NULL;
	} 
	
	if(strcmp(oauth_name, "") == 0) {
		fprintf(stderr, "No value set to 'oauth_name' in config file %s !!\n", filepath);	
		config_destroy(&cfg);
	    	return NULL;
	} 

	if(strcmp(channel_chat, "") == 0) {
		fprintf(stderr, "No value set to 'channel_chat' in config file %s !!\n", filepath);	
		config_destroy(&cfg);
	    	return NULL;
	} 

	struct parsed_cfg *result = malloc(sizeof(*result));

	result->oauth_nick = strdup(oauth_name);
	result->channel = strdup(channel_chat);
	result->commands = true;
	result->tags = true;
	result->oauth = strdup(oauth);

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
