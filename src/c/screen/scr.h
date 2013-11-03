// Joe Jevnik
// 25.10.2013
// Object that handles writing to the screen.

#ifndef SCREEN_H
#define SCREEN_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <pthread.h>

#include "../C_NXT/nxt.h"

#include "logmsg.h"
#include "log_queue.h"

#define path_to_logfile "console_log"
#define MAC_ADDRESS "00:16:53:1A:14:6A"

#define DEFAULT_PAIR 0
#define GREEN_PAIR COLOR_PAIR(1)
#define RED_PAIR COLOR_PAIR(2)
#define YELLOW_PAIR COLOR_PAIR(3)
#define MAGENTA_PAIR COLOR_PAIR(4)

typedef struct {
    motorstate_t *m0,*m1,*m2;
    int mr,mc;
    int opt;
    int logc;
    int lock;
    NXT* nxt;
    log_queue *logq;
    sensorstate_t s0,s1,s2,s3;
    WINDOW *logw,*statw,*ctlw;
    logmsg_t *logv;
    FILE *logf;
    //pthread_t *stay_alive_thread,*log_thread;
} SCR;

SCR *alloc_SCR(NXT*);
void free_SCR(SCR*);
void SCR_refresh();
void SCR_writelnattr(SCR*,char*,int);
void SCR_writelnattr_internals(SCR*,char*,int);
void SCR_printui_static(SCR*);
void SCR_handle_opts(SCR*);
inline void SCR_writeln(SCR*,char*);
inline void SCR_prompt(SCR*,char*);
void SCR_draw_stats(SCR*);
void SCR_draw_menu(SCR*);
void SCR_handle_resize(SCR*);

void *stay_alive_tf(void*);
void *log_tf(void*);

// Push one message with no attributes.
inline void SCR_writeln(SCR* scr,char *str){
    SCR_writelnattr(scr,str,DEFAULT_PAIR);
}

// Push one message and wait for the user to press a key to continue.
inline void SCR_prompt(SCR* scr,char *str){
    SCR_writelnattr(scr,str,A_BOLD);
    getch();
}

#endif /* SCREEN_h */
