#ifndef MESSAGE_BASIC_STRUCTS
#define MESSAGE_BASIC_STRUCTS

#define IRC_MAX_MESSAGE_CHARS 	512
#define IRC_MAX_TAGS_CHARS	8192
#define IRC_MAX_TAGS_MESSAGE 	8704

#define TAG_LIST_SIZE 		25

#include "config.h"

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
	struct tag  	*tag_list;
	char 	 	*raw_msg;
	char 	 	*msg;	
	char 	 	*username;	
   	enum COMMAND 	 command;	
	int 		 is_command;
	struct parsed_cfg *cfg;
};

#endif
