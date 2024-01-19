#include "parse_msg.h"
#include "config.h"

static struct cmd_hash command_hash[] = 
{
	{CLEARMSG , 0xD0F27FF3},
	{GLOBALUSERSTATE, 0x7134EAF6},
	{HOSTTARGET, 0xEF39400A},
	{NOTICE, 0xC39644E7},
	{PRIVMSG, 0xD693152D},
	{JOIN, 0x7C86FD55},
	{PART, 0x7C8A0D3C},
	//{PING, 0x7C8A2EB3,},
	{CAP, 0xB87D899},
	{RECONNECT, 0xD32BF926},
	{ROOMSTATE, 0xA52C87A3,},
	{USERNOTICE, 0x6325C926,},
	{WHISPER, 0xDA158E07},
	{NO_ID, 0xDAF672},
	{ 0, 0 },
};

int parse_message(char *raw_msg, struct message *result) 
{
	char *raw_command = NULL;
	char *raw_source = NULL;
	char *raw_tags = NULL;

	result->raw_msg =  raw_msg;

	if(*raw_msg == '@') {
		raw_tags = strsep(&raw_msg, " "); 
	}

	if(*raw_msg == ':') {
		raw_msg++;
		raw_source = strsep(&raw_msg, " ");
	} else {
		/* No need to continue in case of PING */
		result->command = PING;
		return 0;
	}

	raw_msg = _trim(raw_msg);
	if(strchr(raw_msg, ':') == NULL) {
		raw_command = raw_msg;			
	} else {
		raw_command = strsep(&raw_msg, ":");	
	}

	
	if(_parse_command(raw_command, result))
		return 1;
	else {
		if(raw_tags != NULL) {
			_parse_tags(raw_tags, result);
		}
		_parse_source(raw_source, result);
	}
	if(*raw_msg == '!')
		result->is_command = 1;

	result->msg = raw_msg;
	
	return 0;
}

int _parse_tags(char *raw, struct message *ptr)
{
	char *tmp = NULL;
	char *tmp_result = NULL;
	struct tag *list = ptr->tag_list;

	/* used for tag_list index */
	int i = 0;
	while(raw != NULL && i < TAG_LIST_SIZE) 
	{
		tmp_result = strsep(&raw, ";");
		tmp = strsep(&tmp_result, "=");

		list->name  = tmp;
		list->value = tmp_result;
		
		i++;list++;
	}

	return 0;
}

void _parse_source(char *raw, struct message *result)
{
	char *tmp_result = strsep(&raw, "!");
	result->username = tmp_result;
	return;
}

char *search_tag_name(const char *tag_name, struct tag *tag_list)
{
	const struct tag *tag_ptr = tag_list;		
	int i = 0; /* used as counter for tags list size */

	/* Loop until not more tag's found
	 * or end of tag_list.
	 */
	while(tag_ptr->name != NULL && i < TAG_LIST_SIZE) {
		
		if(!strcmp(tag_name, tag_ptr->name)) {
			return tag_ptr->value;
		}

		/* increment tag's list index 
		 * and size counter.*/
		tag_ptr++; i++;
		continue;
	}
	return NULL;
}

int _parse_command(char *raw, struct message *result)
{
	unsigned long hash_result = 0;
	char *command_part = NULL;
	struct cmd_hash *table = command_hash;

	command_part = strsep(&raw, " ");
	
	if(command_part == NULL)
		return 1;

	hash_result = _hash((unsigned char *)command_part);

	while(table->hash_value != 0 )
	{	
		if(table->hash_value == hash_result) {
			result->command = table->cmd;			
			return 0;
		}
		table++;
	}
	result->command = NO_ID;
	return 1;
}
struct message* message_create(char *np_msg) 
{
	struct message *new = malloc(sizeof(*new));
	new->tag_list = calloc(TAG_LIST_SIZE ,sizeof(struct tag));
	
	if(new->tag_list == NULL) {
		printf("Error on calloc \n");
	}

	if(!(new || new->tag_list)) {
		errno = ENOMEM;
		return NULL;
	}

	new->raw_msg = np_msg;
	new->msg = NULL;
	new->username = NULL;

	new->command = 0;	
	new->is_command = 0;

	new->conn = NULL;

	return new;
}
struct message*
message_create_cfg(char *np_msg,
			struct parsed_cfg *cfg_ptr) 
{
	struct message *new = malloc(sizeof(*new));
	new->tag_list = calloc(TAG_LIST_SIZE ,sizeof(struct tag));
	
	if(new->tag_list == NULL) {
		printf("Error on calloc \n");
	}

	if(!(new || new->tag_list)) {
		errno = ENOMEM;
		return NULL;
	}

	new->raw_msg = np_msg;
	new->msg = NULL;
	new->username = NULL;

	new->command = 0;	
	new->is_command = 0;
	new->cfg = cfg_ptr;

	return new;
}

void message_destroy(struct message** msg) 
{
	free((*msg)->raw_msg);
	free((*msg)->tag_list);
	free(*msg);
	(*msg) = NULL;

	return;
}

char *_ltrim(char *s)
{
    	while(isspace(*s))
	    	s++;
    	return s;
}

char *_rtrim(char *s)
{
	char* back = s + strlen(s);

	/* Loop backwards until 
	 * non space character
	 */
    	while(isspace(*--back));

	/* New 'end' set */
	*(back+1) = '\0';
	return s;
}

char *_trim(char *s)
{
    	return _rtrim(_ltrim(s));
}

// copied from http://www.cse.yorku.ca/~oz/hash.html
unsigned long _hash(unsigned char *str)
{
	unsigned int hash = 5381;
        int c;

        while ((c = *str++))
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}
