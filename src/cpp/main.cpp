#include "screen/Screen.h"
#include <ncurses.h>

int main(){
    Screen scr;
    scr.draw_splash();
    scr.draw_menu();
    scr.writeln("Test");
    getch();
    scr.writeln("test2");
    return 0;
}
