#ifndef MESSAGE_BASIC_STRUCTS
#define MESSAGE_BASIC_STRUCTS

#define IRC_MAX_MESSAGE_CHARS 	512
#define IRC_MAX_TAGS_CHARS	8192
#define IRC_MAX_TAGS_MESSAGE 	8704

#define TAG_LIST_SIZE 		25

#include "config.h"
#include "connect_socket.h"


enum COMMAND { CLEARCHAT ,
       	CLEARMSG,
       	GLOBALUSERSTATE,
       	HOSTTARGET,
       	NOTICE,
       	PRIVMSG,
       	JOIN,
       	PART,
       	PING,
	CAP,
       	RECONNECT,
       	ROOMSTATE,
	USERNOTICE,
	WHISPER,
	NO_ID = 0,
};

struct cmd_hash {
	enum COMMAND cmd;
	unsigned long hash_value;	
};

struct twitch_msg {
	struct irc_msg 	*irc;
	struct T_SSL 	*conn;
	struct p_cfg 	*cfg;
	char 		*username;
	unsigned char 	*rgb;
	enum COMMAND     cmd;
	int 		 is_command;
	int 		 mod;
};

extern struct twitch_msg *
t_msg_alloc();

extern void
t_msg_free(struct twitch_msg **);
#endif
