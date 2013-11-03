// Joe Jevnik
// 31.10.2013
// A type to be held by the message queue.

#ifndef LOGMSG_H
#define LOGMSG_H

typedef struct {
    char *txt;
    int attr;
} logmsg_t;

#endif /* LOGMSG_H */
