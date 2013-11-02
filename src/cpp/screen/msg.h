// Joe Jevnik
// 31.10.2013
// A type to be held by the message queue.

#ifndef MSG_H
#define MSG_H

struct msg{
    char *txt;
    int attr;
};

typedef struct msg msg_t;

#endif /* MSG_H */
