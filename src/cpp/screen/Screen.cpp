// Joe Jevnik
// 25.10.2013
// Implementation of Screen

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <time.h>

#include "../lestat/exceptions.h"
#include "../lestat/comms.h"
#include "../lestat/usbcomm.h"
#include "../lestat/bluecomm.h"
#include "../lestat/message.h"
#include "../lestat/opcodes.h"

#include "Screen.h"
#include "../robot_control/remote.cpp"

#define path_to_logfile "console_log"

Screen::Screen(){
    initscr();
    getmaxyx(stdscr,mr,mc);
    start_color();
    use_default_colors();
    init_pair(1,COLOR_GREEN,-1);
    init_pair(2,COLOR_RED,-1);
    raw();
    keypad(stdscr,TRUE);
    curs_set(0);
    noecho();
    opt = 0;
    logf = fopen(path_to_logfile,"a");
    logc = mr - 2;
    logv = (char**) malloc(sizeof(char*) * logc);
    for (int n = 0;n < logc;n++){
	logv[n] = strdup("");
    }
    m0 = 0;
    m1 = 0;
    m2 = 0;
}

Screen::~Screen(){
    for (int n = 0;n < logc;n++){
	free(logv[n]);
    }
    free(logv);
    fclose(logf);
    delete op;
    endwin();
}

// logs the string to the console, and adds it to the console logfile.
void Screen::writeln(char *str){
    time_t t;
    struct tm *ti;
    free(logv[logc - 1]);
    for (int n = logc - 1;n >= 0;n--){
	logv[n] = logv[n - 1];
    }
    time(&t);
    ti = localtime(&t);
    char *tstr = strdup(str);
    char buffer[4 * mc / 5];
    strftime(buffer,4 * mc / 5,"[%H:%M:%S]:",ti);
    strcat(buffer,tstr);
    logv[0] = strdup(buffer);
    free(tstr);
    fwrite(logv[0],sizeof(char),strlen(logv[0]),logf);
    fputc('\n',logf);
    for (int n = 0;n < logc;n++){
	move(mr - (n + 1),mc / 5 + 1);
	clrtoeol();
	mvprintw(mr - (n + 1),mc / 5 + 1,"%s",logv[n]);
    }
    refresh();
}

// Draws the robot stats.
void Screen::draw_stats(){
    // printing the battery level.
    unsigned short int b = op->getBatteryLevel();
    if (b > 500){ attron(COLOR_PAIR(1)); }
    else { attron(COLOR_PAIR(2)); }
    mvprintw(3,9,"%humV",op->getBatteryLevel());
    if (b > 500){ attroff(COLOR_PAIR(1)); }
    else { attroff(COLOR_PAIR(2)); }
    // printing the motor speeds.
    if (m0 >= 0){ attron(COLOR_PAIR(1)); }
    else { attron(COLOR_PAIR(2)); }
    mvprintw(6,11,"%+03d",m0);
    if (m0 > 0){ attroff(COLOR_PAIR(1)); }
    else { attroff(COLOR_PAIR(2)); }
    if (m1 >= 0){ attron(COLOR_PAIR(1)); }
    else { attron(COLOR_PAIR(2)); }
    mvprintw(7,11,"%+03d",m1);
    if (m1 >= 0){ attroff(COLOR_PAIR(1)); }
    else { attroff(COLOR_PAIR(2)); }
    if (m2 >= 0){ attron(COLOR_PAIR(1)); }
    else { attron(COLOR_PAIR(2)); }
    mvprintw(8,11,"%+03d",m2);
    if (m2 >= 0){ attroff(COLOR_PAIR(1)); }
    else { attroff(COLOR_PAIR(2)); }
    attron(COLOR_PAIR(2));
    if (s0.calibratedValue > 500){ attron(COLOR_PAIR(1)); }
    mvprintw(11,11,"%d",s0.calibratedValue);
    if (s0.calibratedValue > 500){ attroff(COLOR_PAIR(1)); }
    mvprintw(12,11,"%d",s1.calibratedValue);
    mvprintw(13,11,"%d",s2.calibratedValue);
    mvprintw(14,11,"%d",s3.calibratedValue);
    attroff(COLOR_PAIR(2));
}

// Prints the static ui and then prints the main menu.
void Screen::draw_menu(){
    print_ui_static();
    handle_opts();
}

// Draws the splash screen and establishes connection.
void Screen::draw_splash(){
    print_ui_static();
    writeln("Starting session");
    writeln("Press any key to connect...");
    getch();
    writeln("Attempting to connect...");
    refresh();
    try{
	nxt.connect("00:16:53:1A:14:6A");
    } catch(NxtEx &ex){
        writeln("ERROR: failed to connect!");
        writeln("Press any key to continue...");
	getch();
	endwin();
	exit(1);
    }
    writeln("Connection established!");
    writeln("Grabbing initial sensor readings...");
    op = new Opcodes(&nxt);
    s0 = op->getInputValues(0);
    s1 = op->getInputValues(1);
    s2 = op->getInputValues(2);
    s3 = op->getInputValues(3);
    writeln("Ready!");
    refresh();
}

// Prints the basic UI features, they will be populated by the poll thread.
void Screen::print_ui_static(){
    attron(A_BOLD);
    mvprintw(0,mc - 11,"NXT GROUP 9");
    attroff(A_BOLD);
    mvprintw(3,0,"Battery:");
    mvprintw(5,0,"Motors:");
    mvprintw(6,2,"Motor_1:");
    mvprintw(7,2,"Motor_2:");
    mvprintw(8,2,"Motor_3:");
    mvprintw(10,0,"Sensors:");
    mvprintw(11,2,"Optical:");
    mvprintw(12,2,"Push_1:");
    mvprintw(13,2,"Push_2:");
    mvprintw(14,2,"RGB:");
    int c = mc / 5;
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
    attron(A_BOLD);
    if (opt == 0){ attron(A_STANDOUT); }
    mvprintw(mr - 3,0,"REMOTE");
    if (opt == 0){ attroff(A_STANDOUT); }
    if (opt == 1){ attron(A_STANDOUT); }
    mvprintw(mr - 2,0,"RIGHT");
    if (opt == 1){ attroff(A_STANDOUT); }
    if (opt == 2){ attron(A_STANDOUT); }
    mvprintw(mr - 1,0,"LEFT");
    if (opt == 2){ attroff(A_STANDOUT); }
    attroff(A_BOLD);
    refresh();
    int logc_temp;
    char **logv_temp;
    switch(getch()){
    case 3:
	for (int n = 0;n < logc;n++){
	    free(logv[n]);
	}
	free(logv);
	endwin();
	exit(0);
	break;
    case KEY_RESIZE:
	endwin();
	getmaxyx(stdscr,mr,mc);
	logc_temp = logc;
	logc = 4 * mc / 5 - 1;
	logv_temp = (char**) malloc(sizeof(char*) * logc);
	for (int n = 0;n < logc;n++){
	    logv_temp[n] = logv[n];
	}
	free(logv);
	logv = logv_temp; 
	for (int n = logc_temp;n < logc;n++){
	    logv[n] = strdup("");
	}
	clear();
	refresh();
	draw_menu();
	break;
    case KEY_UP:
	if (opt > 0){
	--opt;
	}
	break;
    case KEY_DOWN:
	if (opt < 2){
	    ++opt;
	}
	break;
    case 10: // ENTER
	switch(opt){
	case 0:
	    attron(A_BOLD);
	    mvprintw(mr - 3,0,"      ");
	    mvprintw(mr - 2,0,"      ");
	    mvprintw(mr - 1,0,"      ");
	    mvprintw(mr - 8,0,"p - return");
	    mvprintw(mr - 7,0,"r - read sensors");
	    mvprintw(mr - 6,0,"SPACE - stop");
	    mvprintw(mr - 4,4,"w");
	    mvprintw(mr - 3,1,"q     e");
	    mvprintw(mr - 2,1,"a     d");
	    mvprintw(mr - 1,4,"s");
	    attroff(A_BOLD);
	    writeln("Starting remote control!");
	    r_remote(this);
	    return;
	    break;
	case 1:
	    writeln("Starting right side autonomous control");
	    //r_left(this);
	    //return;
	    break;
	case 2:
	    writeln("Starting left side autonomous control");
	    //r_right(this);
	    //return;
	    break;
	}
	break;
    }
    handle_opts();
}
