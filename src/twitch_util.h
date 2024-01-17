#ifndef TWITCH_UTIL
#define TWITCH_UTIL

#include "message.h"
#include "config.h"
#include "connect_socket.h"
#include "parse_msg.h"

extern int respond_message(struct T_SSL *, struct message *, const char *);

#endif
