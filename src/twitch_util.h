#ifndef TWITCH_UTIL
#define TWITCH_UTIL

#define _GNU_SOURCE

#include "message.h"
#include "config.h"
#include "connect_socket.h"
#include "irc_parse.h"

extern int respond_message(struct T_SSL *, struct twitch_msg*, const char *);

#endif
