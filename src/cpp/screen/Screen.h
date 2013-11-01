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


#include <queue>

#include "../lestat/bluecomm.h"
#include "../lestat/opcodes.h"

#include "msg_t.h"

#define path_to_logfile "console_log"
#define MAC_ADDRESS "00:16:53:1A:14:6A"

#define DEFAULT_PAIR 0
#define GREEN_PAIR COLOR_PAIR(1)
#define RED_PAIR COLOR_PAIR(2)
#define YELLOW_PAIR COLOR_PAIR(3)

class Screen{
public:
    Screen(BlueComm*);
    ~Screen();
    void scr_refresh();
    inline void writelnattr(char*,int);
    inline void writeln(char*);
    inline void prompt(char*);
    void draw_stats();
    void draw_menu();
    BlueComm *nxt;
    int m0,m1,m2;
    SensorState s0,s1,s2,s3;
    Opcodes *op;
    friend void *log_tf(void*);
private:
    void writelnattr_internals(char*,int);
    void print_ui_static();
    void handle_opts();
    WINDOW *logw,*statw,*ctlw;
    std::queue<struct msg_t*> log;
    struct msg_t *logv;
    int mr,mc,opt,logc;
    FILE *logf;
    bool lock;
    pthread_t stay_alive_thread,log_thread;
};

void *stay_alive_tf(void*);
void *log_tf(void*);

// Push one message into the message queue that will allow it to be processed
// outside of the logic thread.
inline void Screen::writelnattr(char *str,int attr){
    struct msg_t *msg = (struct msg_t*) malloc(sizeof(struct msg_t));
    msg->txt = str;
    msg->attr = attr;
    log.push(msg);
}

// Push one message with no attributes.
inline void Screen::writeln(char *str){
    struct msg_t *msg = (struct msg_t*) malloc(sizeof(struct msg_t));
    msg->txt = str;
    msg->attr = DEFAULT_PAIR;
    log.push(msg);
}

// Push one message and wait for the user to press a key to continue.
inline void Screen::prompt(char *str){
    struct msg_t *msg = (struct msg_t*) malloc(sizeof(struct msg_t));
    msg->txt = str;
    msg->attr = A_BOLD;
    log.push(msg);
    getch();
}

#endif /* SCREEN_h */
