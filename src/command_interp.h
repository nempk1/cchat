#ifndef _COMMAND_INTERP_
#define _COMMAND_INTERP_

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "queue.h"
#include "parse_msg.h"
#include "connect_socket.h"
#include "parse_msg.h"
#include "twitch_util.h"

extern int cmd_interp(struct message*);

#endif
