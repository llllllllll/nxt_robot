// Joe Jevnik
// 25.10.2013
// Implementation of Screen

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

#include "../lestat/exceptions.h"
#include "../lestat/comms.h"
#include "../lestat/usbcomm.h"
#include "../lestat/bluecomm.h"
#include "../lestat/message.h"
#include "../lestat/opcodes.h"

#include "Screen.h"

Screen::Screen(){
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
    logf = fopen("console_log","a");
    logc = mr - 2;
    logv = (char**) malloc(sizeof(char*) * logc);
    for (int n = 0;n < logc;n++){
	logv[n] = strdup("");
    }
}

Screen::~Screen(){
    for (int n = 0;n < logc;n++){
	free(logv[n]);
    }
    free(logv);
    endwin();
}

void Screen::writeln(char *str){
    free(logv[logc - 1]);
    for (int n = logc - 1;n >= 0;n--){
	logv[n] = logv[n - 1];
    }
    logv[0] = strdup(str);
    fwrite(logv[0],sizeof(char),sizeof(logv[0]),logf);
    fputc('\n',logf);
    for (int n = 0;n < logc;n++){
	move(mr - (n + 1),mc / 3 + 1);
	clrtoeol();
	mvprintw(mr - (n + 1),mc / 3 + 1,"%s",logv[n]);
    }
    refresh();
}

void Screen::draw_stats(){
    Opcodes op(&nxt);
     mvprintw(3,9,"%humV",op.getBatteryLevel());
}
    

// Prints the static ui and then prints the main menu.
void Screen::draw_menu(){
    print_ui_static();
    handle_opts();
}

// Draws the splash screen and
void Screen::draw_splash(){
    print_ui_static();
    attron(A_BOLD);
    writeln("Press any key to connect...");
    getch();
    attroff(A_BOLD);
    print_ui_static();
    attron(A_BOLD);
    writeln("Attempting to connect...");
    attroff(A_BOLD);
    refresh();
    try{
	nxt.connect("00:16:53:1A:14:6A");
    } catch(NxtEx &ex){
	clear();
	attron(A_BOLD);
        writeln("ERROR: failed to connect!");
	attroff(A_BOLD);
        writeln("Press any key to continue...");
	getch();
	endwin();
	exit(1);
    }
    clear();
    print_ui_static();
    attron(A_BOLD);
    writeln("Connection established!");
    attroff(A_BOLD);
    refresh();
    sleep(1);
    clear();
    refresh();
}

// Prints the basic UI features, they will be populated by the poll thread.
void Screen::print_ui_static(){
    attron(A_BOLD);
    mvprintw(0,mc - 11,"NXT GROUP 9");
    attroff(A_BOLD);
    mvprintw(3,0,"Battery: ");
    int c = mc / 3;
    for (int n = 2;n < mr;n++){
	mvprintw(n,c,"|");
    }
    for (int n = 0;n < mc;n++){
	mvprintw(1,n,"_");
    }
    attron(A_UNDERLINE);
    mvprintw(1 ,0,"Robot Status:");
    mvprintw(1,c + 1,"Log:");
    attroff(A_UNDERLINE);
}

// Prints the options and handles user input.
void Screen::handle_opts(){
    draw_stats();
    if (opt == 0){ attron(A_STANDOUT); }
    mvprintw(5,mc / 4 - 3,"REMOTE");
    if (opt == 0){ attroff(A_STANDOUT); }
    if (opt == 1){ attron(A_STANDOUT); }
    mvprintw(5,mc / 2 - 2,"RIGHT");
    if (opt == 1){ attroff(A_STANDOUT); }
    if (opt == 2){ attron(A_STANDOUT); }
    mvprintw(5,3 * mc / 4 - 2,"LEFT");
    if (opt == 2){ attroff(A_STANDOUT); }
     refresh();
    switch(getch()){
    case KEY_RESIZE:
	endwin();
	getmaxyx(stdscr,mr,mc);
	for (int n = 0;n < logc;n++){
	    free(logv[n]);
	}
	free(logv);
	logc = mc / 3 - 1;
	logv = (char**) malloc(sizeof(char*) * logc);
	refresh();
	draw_menu();
	break;
    case KEY_LEFT:
	if (opt > 0){
	--opt;
	}
	break;
    case KEY_RIGHT:
	if (opt < 2){
	    ++opt;
	}
	break;
    case 10: // ENTER
	switch(opt){
	case 0:
	    writeln("test 0");
	    //start_remote(this);
	    //return;
	    break;
	case 1:
	    writeln("test 1");
	    //start_left(this);
	    //return;
	    break;
	case 2:
	    writeln("test 2");
	    //start_right(this);
	    //return;
	    break;
	}
	break;
    }
    handle_opts();
}
