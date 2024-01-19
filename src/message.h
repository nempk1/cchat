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

struct tag {
	char 	*name;
	char 	*value;
};

struct message {
	struct parsed_cfg *cfg;
	struct tag  	*tag_list;
	struct T_SSL 	*conn;
	char 	 	*raw_msg;
	char 	 	*msg;	
	char 	 	*username;	
	unsigned char	*rgb_8bit;
   	enum COMMAND 	 command;	
	int 		 is_command;
	int 		 mod;
};

#endif
