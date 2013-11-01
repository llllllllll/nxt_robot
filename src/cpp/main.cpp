#include "screen/Screen.h"
#include <ncurses.h>

int main(){
    BlueComm nxt;
    Screen scr(&nxt);
    scr.draw_menu();
    return 0;
}
