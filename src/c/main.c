// Joe Jevnik
// 1.11.2013
// Main function.

#include <ncurses.h>
#include <assert.h>

#include "C_NXT/nxt.h"

#include "screen/scr.h"


int main(){
    //pthread_t log_thread,stay_alive_thread;
    NXT *nxt = alloc_NXT();
    SCR *scr = alloc_SCR(nxt);
    //pthread_create(&stay_alive_thread,NULL,stay_alive_tf, scr);
    // pthread_create(&log_thread,NULL,log_tf,scr);
    assert(scr);
    SCR_draw_menu(scr);
    return 0;
}
