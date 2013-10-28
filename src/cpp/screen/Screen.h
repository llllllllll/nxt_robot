// Joe Jevnik
// 25.10.2013
// Object that handles writing to the screen.

#ifndef SCREEN_H
#define SCREEN_H
#include "../lestat/bluecomm.h"
#include <stdio.h>

class Screen{
public:
    Screen();
    ~Screen();
    void writeln(char*);
    void draw_stats();
    void draw_menu();
    void draw_splash();
    BlueComm nxt; // public to allow other functions to modify it.
private:
    void print_ui_static();
    void handle_opts();
    int mr,mc,opt,logc;
    char **logv;
    FILE *logf;
};

#endif /* SCREEN_h */
