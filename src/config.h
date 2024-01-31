#ifndef _CCHAT_CONFIG_
#define _CCHAT_CONFIG_

#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <libconfig.h>

#include "connect_socket.h"


#define OAUTH_PREFIX "PASS oauth:"
#define NICK_PREFIX "NICK "
#define JOIN_PREFIX "JOIN #"
#define ENABLE_ALL "CAP REQ :twitch.tv/commands twitch.tv/tags twitch.tv/membership\n"
#define ENABLE_TAGS "CAP REQ :twitch.tv/tags\n"
#define ENABLE_COMMANDS "CAP REQ :twitch.tv/commands\n"
#define ENABLE_MEMBERSHIP "CAP REQ :twitch.tv/membership\n"


struct p_cfg {
	char *channel;
	char *oauth;
	char *oauth_nick;
	int tags;
	int commands;
};

extern int
config_send(struct T_SSL *, const char *, const char *);  

extern struct p_cfg *
config_parse_file(const char *);
#endif
