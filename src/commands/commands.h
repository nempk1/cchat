#ifndef COMMANDS_H
#define COMMANDS_H

#include "parse_msg.h"
#include "connect_socket.h"
#include "message.h"
#include "twitch_util.h"

struct command_str {
	const char *cmd_str;
	void* (*fun_ptr)(void*);
};

struct command_hash {
	unsigned long hash;
	void* (*fun_ptr)(void*);
};

/* Creating custom command bellow */
extern void *
test_command(void *data)
{
	struct message *msg = data;

	respond_message(msg->conn, msg, "test command worked");
	return NULL;
}

#endif
