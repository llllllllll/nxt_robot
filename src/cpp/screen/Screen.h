// Joe Jevnik
// 25.10.2013
// Object that handles writing to the screen.

#ifndef SCREEN_H
#define SCREEN_H
#include <stdio.h>

#include "../lestat/bluecomm.h"
#include "../lestat/opcodes.h"

class Screen{
public:
    Screen();
    ~Screen();
    void writelnattr(char*,int);
    inline void writeln(char*);
    void draw_stats();
    void draw_menu();
    void draw_splash();
    BlueComm nxt; // public to allow other functions to modify it.
    int m0,m1,m2;
    SensorState s0,s1,s2,s3;
    Opcodes *op;
private:
    void print_ui_static();
    void handle_opts();
    int mr,mc,opt,logc,*logattr;
    char **logv;
    FILE *logf;
};

#endif /* SCREEN_h */

/*

chars:
  ─
  │
  ▒

*/
