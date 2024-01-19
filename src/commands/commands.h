#ifndef COMMANDS_H
#define COMMANDS_H

#include <sys/resource.h>
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

extern void *
memory_usage(void *data)
{
	char *response = NULL;
	struct message *msg = data;
	struct rusage resource;
	getrusage(RUSAGE_SELF, &resource);
	asprintf(&response, "Memory Usage: %ld kB", resource.ru_maxrss);
	respond_message(msg->conn, msg, response);
	free(response);
	return NULL;

}


#endif
