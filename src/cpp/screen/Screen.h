// Joe Jevnik
// 25.10.2013
// Object that handles writing to the screen.

#ifndef SCREEN_H
#define SCREEN_H
#include <stdio.h>
#include <pthread.h>
#include <queue>

#include "../lestat/bluecomm.h"
#include "../lestat/opcodes.h"

#include "msg_t.h"

class Screen{
public:
    Screen();
    ~Screen();
    void writelnattr(char*,int);
    inline void writeln(char*);
    void draw_stats();
    void draw_menu();
    BlueComm nxt;
    int m0,m1,m2;
    SensorState s0,s1,s2,s3;
    Opcodes *op;
    friend void *log_tf(void*);
private:
    void writelnattr_internals(char*,int);
    void print_ui_static();
    void handle_opts();
    std::queue<msg_t*> log;
    int mr,mc,opt,logc,*logattr;
    char **logv;
    FILE *logf;
    bool lock;
    pthread_t stay_alive_thread,log_thread;
};

void *stay_alive_tf(void*);
void *log_tf(void*);

#endif /* SCREEN_h */
