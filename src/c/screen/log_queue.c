// Joe Jevnik
// 3.11.2013
// logmsgqueue

#include <stdlib.h>

#include "log_queue.h"

void insert_tail(struct logmsg_list *l,logmsg_t *msg){
    if (!l){
	struct logmsg_list *t = malloc(sizeof(struct logmsg_list));
	t->data = msg;
	t->tail = NULL;
	l = t;
	return;
    }
    insert_tail(l->tail,msg);
}

// Push one msg into the queue.
void qpush(log_queue *q,logmsg_t *msg){
    insert_tail(q->head,msg);
}

logmsg_t *qpop(log_queue *q){
    if (qempty(q)){
	return NULL;
    }
    struct logmsg_list *tmp = q->head->tail;
    logmsg_t *msg = q->head->data;
    free(q->head);
    q->head = tmp;
    return msg;
}

int qempty(log_queue *q){
    return (int) !q->head;
}
