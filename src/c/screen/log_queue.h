// Joe Jevnik
// 3.11.2013
// A queue for holding logmsg_ts.

#ifndef LOGMSG_QUEUE_H
#define LOGMSG_QUEUE_H

#include "logmsg.h"

struct logmsg_list {
    logmsg_t *data;
    struct logmsg_list *tail;
};

typedef struct {
    struct logmsg_list *head;
} log_queue;

void insert_tail(struct logmsg_list*,logmsg_t*);
void qpush(log_queue*,logmsg_t*);
logmsg_t *qpop(log_queue*);
int qempty(log_queue*);

#endif /* LOGMSG_QUEUE_H */
