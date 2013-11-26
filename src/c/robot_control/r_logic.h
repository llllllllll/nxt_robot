// Joe Jevnik
// 3.11.2013
// robot logic functions.

#ifndef R_LOGIC_H
#define R_LOGIC_H

#include "../screen/scr.h"
#include "../C_NXT/nxt.h"

static clock_t _current_time = 0;

struct timer_flag{
    SCR *scr;
    int secs;
    int *flag;
};

void start_timer(SCR*,int,int*);
int current_time();
void* g_start_timer(void*);

void r_remote(SCR*,NXT*);
void r_left(SCR*,NXT*);
void r_right(SCR*,NXT*);

#endif /* R_LOGIC_H */
