#ifndef _CCHAT_CONFIG_
#define _CCHAT_CONFIG_

#include <stdbool.h>
#include <stdio.h>

#include <libconfig.h>

#include "connect_socket.h"


#define OAUTH_PREFIX "PASS oauth:"
#define NICK_PREFIX "NICK "
#define JOIN_PREFIX "JOIN #"
#define ENABLE_ALL "CAP REQ :twitch.tv/tags commands\r\n"
#define ENABLE_TAGS "CAP REQ :twitch.tv/tags\r\n"
#define ENABLE_COMMANDS "CAP REQ :twitch.tv/commands\r\n"


struct parsed_cfg {
	char *channel;
	char *oauth;
	char *oauth_nick;
	int tags;
	int commands;
};

extern int
config_send(struct T_SSL *, const char *);  

extern struct parsed_cfg *
config_parse_file(const char *);
#endif
