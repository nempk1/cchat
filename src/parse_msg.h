#ifndef _PARSE_MSG_
#define _PARSE_MSG_

#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "basic.h"
#include "message.h"
#include "queue.h"

#define MAX_TAG_VALUE_SIZE 	50	
#define MAX_TAG_NAME_SIZE 	16

#define NUM_OF_TAGS 		25
#define MAX_MESSAGE_BSIZE 	8704


// DEFINITION OF TYPES OF MESSAGES RECEIVED

extern int
parse_message(char *, struct message *);

extern int
_parse_command(char *, struct message *);

extern char *
search_tag_name(const char *, struct tag *);

extern int _parse_tags(char *, struct message*);
extern void _parse_source(char *, struct message*);
extern unsigned long _hash(unsigned char *);

extern struct message* message_create(char *); 
extern void message_destroy(struct message **);

extern char *_ltrim(char *);
extern char *_rtrim(char *);
extern char *_trim(char *);

extern unsigned char* hexstr_to_char(const char* );
#endif
