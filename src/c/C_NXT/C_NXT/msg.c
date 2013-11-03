// Joe Jevnik
// 2.11.2013
// implementation of the msg_t.

#include <stdlib.h>
#include <string.h>

#include "msg.h"

typedef unsigned char u_char;

// allocates and initializes a new msg_t.
msg_t *alloc_msg(char type, char opcode){
    msg_t *tmp = malloc(sizeof(msg_t));
    tmp->txt = malloc(2 * sizeof(u_char));
    if (!tmp){
	return NULL;
    }
    tmp->txt[0] = type;
    tmp->txt[1] = opcode;
    tmp->len = 2;
    return tmp;
}

void free_msg(msg_t *msg){
    free(msg->txt);
    free(msg);
}

// equivelent to { free(msg); msg = alloc(type,opcode); }.
void msg_reset(msg_t *msg,char type, char opcode){
    free(msg->txt);
    msg->txt = malloc(2 * sizeof(u_char));
    msg->txt[0] = type;
    msg->txt[1] = opcode;
    msg->len = 2;
}

// adds a signed char to the end of msg.
void msg_addS8(msg_t *msg,signed char a){
    u_char *tmp = realloc(msg->txt,(msg->len + 1) * sizeof(u_char));
    if (!tmp){
	return;
    }
    msg->txt = tmp;
    msg->len += 1;
    msg->txt[msg->len - 1] = (u_char) a;
}

// add an u_char to the msg.
void msg_addU8(msg_t *msg,u_char a){
    u_char *tmp = realloc(msg->txt,(msg->len + 1) * sizeof(u_char));
    if (!tmp){
	return;
    }
    msg->txt = tmp;
    msg->len += 1;
    msg->txt[msg->len - 1] = a;
}

// add a signed short to the msg.
void msg_addS16(msg_t *msg,signed short int a){
    u_char *tmp = realloc(msg->txt,(msg->len + 2) * sizeof(u_char));
    if (!tmp){
	return;
    }
    msg->txt = tmp;
    msg->len += 2;
#ifdef _NXT_LITTLE_ENDIAN
    msg->txt[msg->len - 2] = (u_char) (a & 0xff);
    msg->txt[msg->len - 1] = (u_char) ((a >> 8) & 0xff);
#elif _NXT_BIG_ENDIAN
    msg->txt[msg->len - 2] = (u_char) ((a >> 8) & 0xff);
    msg->txt[msg->len - 1] = (u_char) (a & 0xff);
#endif
}

// add an unsinged short the msg.
void msg_addU16(msg_t *msg,unsigned short int a){
    u_char *tmp = realloc(msg,(msg->len + 2) * sizeof(u_char));
    if (!tmp){
	return;
    }
    msg->txt = tmp;
    msg->len += 2;
#ifdef _NXT_LITTLE_ENDIAN
    msg->txt[msg->len - 2] = (u_char) (a & 0xff);
    msg->txt[msg->len - 1] = (u_char) ((a>>8) &0xff);
#elif _NXT_BIG_ENDIAN
    msg->txt[msg->len - 2] = (u_char) ((a >> 8) & 0xff);
    msg->txt[msg->len - 1] = (u_char) (a & 0xff);
#endif
}


// add a signed int to the msg.
void msg_addS32(msg_t *msg,signed int a){
    u_char *tmp = realloc(msg,(msg->len + 4) * sizeof(u_char));
    if (!tmp){
	return;
    }
    msg->txt = tmp;
    msg->len += 4;
#ifdef _NXT_LITTLE_ENDIAN
    msg->txt[msg->len - 4] = (u_char) (a & 0xff);
    msg->txt[msg->len - 3] = (u_char) ((a >> 8) & 0xff);
    msg->txt[msg->len - 2] = (u_char) ((a >> 16) & 0xff);
    msg->txt[msg->len - 1] = (u_char) ((a >> 24) & 0xff);
#elif _NXT_BIG_ENDIAN
    msg->txt[msg->len - 4] = (u_char) ((a >> 24) & 0xff);
    msg->txt[msg->len - 3] = (u_char) ((a >> 16) & 0xff);
    msg->txt[msg->len - 2] = (u_char) ((a >> 8) & 0xff);
    msg->txt[msg->len - 1] = (u_char) (a & 0xff);
#endif
}

// add an unsigned int to the msg.
void msg_addU32(msg_t *msg,unsigned int a){
    u_char *tmp = realloc(msg,(msg->len + 4) * sizeof(u_char));
    if (!tmp){
	return;
    }
    msg->txt = tmp;
    msg->len += 4;
#ifdef _NXT_LITTLE_ENDIAN
    msg->txt[msg->len - 4] = (u_char) (a & 0xff);
    msg->txt[msg->len - 3] = (u_char) ((a >> 8) & 0xff);
    msg->txt[msg->len - 2] = (u_char) ((a >> 16) & 0xff);
    msg->txt[msg->len - 1] = (u_char) ((a >> 24) & 0xff);
#elif _NXT_BIG_ENDIAN
    msg->txt[msg->len - 4] = (u_char) ((a >> 24) & 0xff);
    msg->txt[msg->len - 3] = (u_char) ((a >> 16) & 0xff);
    msg->txt[msg->len - 2] = (u_char) ((a >> 8) & 0xff);
    msg->txt[msg->len - 1] = (u_char) (a & 0xff);
#endif
}

// converts a message to a signed short int.
signed short int msg_bytes2S16(char *str){
    return (signed short int) ((0xff & str[0]) 
			       | ((0xff & str[1]) << 8));
}

// converts a message to an unsigned short int.
unsigned short int msg_bytes2U16(char *str){;
    return (unsigned short int) ((0xff & str[0]) 
				 | ((0xff & str[1]) << 8));
}

// converts a message to a signed int.
signed int msg_bytes2S32(char *str){
    return (signed int) ((0xff & str[0])
			 | ((0xff & str[1]) << 8)
			 | ((0xff & str[2]) << 16)
			 | ((0xff & str[3]) << 24));
}

// converts a message an unsigned int.
unsigned int msg_bytes2U32(char *str){
    return (unsigned int) ((0xff & str[0])
			   | ((0xff & str[1]) << 8)
			   | ((0xff & str[2]) << 16)
			   | ((0xff & str[3]) << 24));
}
