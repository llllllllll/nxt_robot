// Joe Jevnik
// 1.11.2013
// Main function.

#include <ncurses.h>
#include <assert.h>

#include "screen/Screen.h"


int main(){
    cpu_set_t proc;
    CPU_ZERO(&proc);
    CPU_SET(0,&proc);
    assert(!pthread_setaffinity_np(pthread_self(),sizeof(cpu_set_t),&proc));
    BlueComm nxt;
    Screen scr(&nxt);
    scr.draw_menu();
    return 0;
}
