#include "screen/Screen.h"
#include <ncurses.h>

int main(){
    Screen scr;
    scr.draw_splash();
    scr.draw_menu();

    return 0;
}