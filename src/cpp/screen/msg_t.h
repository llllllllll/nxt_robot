// Joe Jevnik
// 31.10.2013
// A type to be held by the message queue.

#ifndef MSG_H
#define MSG_H

struct msg_t{
    msg_t(char*,int);
    char *txt;
    int attr;
};

#endif /* MSG_H */
