// Joe Jevnik
// 1.11.2013
// Main function.

#include <ncurses.h>
#include <assert.h>

#include "screen/Screen.h"


int main(){
    BlueComm nxt;
    Screen scr(&nxt);
    scr.draw_menu();
    return 0;
}
