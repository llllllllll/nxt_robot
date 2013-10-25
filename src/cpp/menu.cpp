// Joe Jevnik
// 25.10.2013

// ncurses menus and UI functions.

#include <ncurses.h>
#include <stdlib.h>
#include <iostream>

#include "exceptions.h"
#include "comms.h"
#include "usbcomm.h"
#include "bluecomm.h"
#include "message.h"
#include "opcodes.h"

int mr,mc;

unsigned char opt;

// Initializes the stdscr and sets up the menu.
void init_menus(){
    initscr();
    getmaxyx(stdscr,mr,mc);
    start_color();
    use_default_colors();
    init_pair(1,COLOR_GREEN,-1);
    init_pair(1,COLOR_RED,-1);
    cbreak();
    keypad(stdscr,TRUE);
    curs_set(0);
    noecho();
    opt = 0;
}

// Prints the splash and waits for the user to press akey before continuing.
void print_splash(Comms &nxt){
    attron(A_BOLD);
    mvprintw(mr / 2,mc / 2 - 5,"NXT GROUP 9");
    mvprintw(mr / 2 + 1,mc / 2 - 14,"Press any key to connect...");
    getch();
    mvprintw(mr / 2 + 1,mc / 2 - 14,"  Attempting to connect...");
    attroff(A_BOLD);
    try{
        nxt.connect("00:16:53:1A:14:6A");
    } catch(NxtEx &ex){
	clear();
	attron(A_BOLD);
	mvprintw(mr / 2,mc / 2 - 13,"ERROR: failed to connect!");
	attroff(A_BOLD);
	mvprintw(0,mc - 28,"Press any key to continue...");
	getch();
	endwin();
        exit(1);
    }
    clear();
    attron(A_BOLD);
    mvprintw(mr / 2,mc / 2 - 14,"Connection established");
    attroff(A_BOLD);
    mvprintw(0,mc - 28,"Press any key to continue...");
    refresh();
    getch();
    clear();
    refresh();
}

// Prints the basic UI features, they will be populated by the poll thread.
void print_ui_static(){
    attron(A_BOLD);
    mvprintw(0,mc / 2 - 5,"NXT GROUP 9");
    attroff(A_BOLD);
    mvprintw(0,0,"Battery: ");
    mvprintw(1,0,"Signal:  ");
    int c = mc / 3, r = mr / 2;
    for (int n = mr / 2;n < mr;n++){
	mvprintw(n,c,"|");
    }
    for (int n = 0;n < mc;n++){
	mvprintw(r,n,"_");
    }
    attron(A_UNDERLINE);
    mvprintw(mr / 2 ,0,"Robot Status:");
    mvprintw(mr / 2,c + 1,"Log:");
    attroff(A_UNDERLINE);
				    
}
    

int main(){
    BlueComm nxt;
    init_menus();
    print_splash(nxt);
    print_ui_static();
    getch();
    echo();
    endwin();
    return EXIT_SUCCESS;
}
