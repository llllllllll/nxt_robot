// Joe Jevnik
// 25.10.2013
// Implementation of Screen.

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
#include "msg_t.h"
#include "../robot_control/remote.cpp"

#define path_to_logfile "console_log"
#define MAC_ADDRESS "00:16:53:1A:14:6A"

#define DEFAULT_PAIR 0
#define GREEN_PAIR COLOR_PAIR(1)
#define RED_PAIR COLOR_PAIR(2)
#define YELLOW_PAIR COLOR_PAIR(3)

Screen::Screen(){
    initscr();
    getmaxyx(stdscr,mr,mc);
    start_color();
    use_default_colors();
    init_pair(1,COLOR_GREEN,-1);
    init_pair(2,COLOR_RED,-1);
    init_pair(3,COLOR_YELLOW,-1);
    raw();
    keypad(stdscr,TRUE);
    curs_set(0);
    noecho();
    opt = 0;
    logf = fopen(path_to_logfile,"a");
    logc = mr - 2;
    logv = (char**) malloc(sizeof(char*) * logc);
    logattr = (int*) malloc(sizeof(int) * logc);
    for (int n = 0;n < logc;n++){
	logv[n] = strdup("");
	logattr[n] = 0;
    }
    m0 = 0;
    m1 = 0;
    m2 = 0;
    lock = false;
    print_ui_static();
    writeln("Starting session");
    writelnattr("Press any key to connect...",A_BOLD);
    getch();
    writeln("Attempting to connect...");
    refresh();
    try{
	nxt.connect(MAC_ADDRESS);
    } catch(NxtEx &ex){
        writelnattr("ERROR: failed to connect!",RED_PAIR | A_BOLD);
        writelnattr("Press any key to continue...",A_BOLD);
	getch();
	endwin();
	exit(1);
    }
    writelnattr("Connection established!",GREEN_PAIR);
    writeln("Grabbing initial sensor readings...");
    op = new Opcodes(&nxt);
    s0 = op->getInputValues(0);
    s1 = op->getInputValues(1);
    s2 = op->getInputValues(2);
    s3 = op->getInputValues(3);
    writelnattr("Ready!",GREEN_PAIR | A_BOLD);
    op->setInputMode(0,LIGHT_ACTIVE,BOOLEANMODE,false,NULL);
    pthread_create(&stay_alive_thread,NULL,stay_alive_tf,(void*) this);
    pthread_create(&log_thread,NULL,log_tf,(void*) this);
    refresh();
}

Screen::~Screen(){
    for (int n = 0;n < logc;n++){
	free(logv[n]);
    }
    free(logv);
    free(logattr);
    fclose(logf);
    delete op;
    pthread_cancel(stay_alive_thread);
    pthread_cancel(log_thread);
    endwin();
}

// logs the string to the console with a given attribute,
// and adds it to the console logfile.
void Screen::writelnattr(char *str,int attr){
    log.push(new msg_t(str,attr));
}

// logs to the console with no attributes.
inline void Screen::writeln(char *str){
    writelnattr(str,DEFAULT_PAIR);
}

// Draws the robot stats.
void Screen::draw_stats(){
    while(lock);
    lock = true;
    unsigned short int b = op->getBatteryLevel();
    if (b > 500){ attron(GREEN_PAIR); }
    else { attron(RED_PAIR); }
    mvprintw(3,9,"%humV",op->getBatteryLevel());
    if (b > 500){ attroff(GREEN_PAIR); }
    else { attroff(RED_PAIR); }
    if (m0 >= 0){ attron(GREEN_PAIR); }
    else { attron(RED_PAIR); }
    mvprintw(6,11,"%+04d",m0);
    if (m0 > 0){ attroff(GREEN_PAIR); }
    else { attroff(RED_PAIR); }
    if (m1 >= 0){ attron(GREEN_PAIR); }
    else { attron(RED_PAIR); }
    mvprintw(7,11,"%+04d",m1);
    if (m1 >= 0){ attroff(GREEN_PAIR); }
    else { attroff(RED_PAIR); }
    if (m2 >= 0){ attron(GREEN_PAIR); }
    else { attron(RED_PAIR); }
    mvprintw(8,11,"%+04d",m2);
    if (m2 >= 0){ attroff(GREEN_PAIR); }
    else { attroff(RED_PAIR); }

    if (s0.calibratedValue > 500){ attron(GREEN_PAIR); }
    else { attron(RED_PAIR); }
    mvprintw(11,11,"%d",s0.calibratedValue);
    if (s0.calibratedValue > 500){ attroff(GREEN_PAIR); }
    else { attroff(RED_PAIR); }
    if (s1.calibratedValue > 500){ attron(GREEN_PAIR); }
    else { attron(RED_PAIR); }
    mvprintw(12,11,"%d",s1.calibratedValue);
    if (s1.calibratedValue > 500){ attroff(GREEN_PAIR); }
    else { attroff(RED_PAIR); }
    if (s2.calibratedValue > 500){ attron(GREEN_PAIR); }
    else { attron(RED_PAIR); }
    mvprintw(13,11,"%d",s2.calibratedValue);
    if (s2.calibratedValue > 500){ attroff(GREEN_PAIR); }
    else { attroff(RED_PAIR); }
    if (s3.calibratedValue > 500){ attron(GREEN_PAIR); }
    else { attron(RED_PAIR); }
    mvprintw(14,11,"%d",s3.calibratedValue);
    if (s3.calibratedValue > 500){ attroff(GREEN_PAIR); }
    else { attroff(RED_PAIR); }
    attroff(RED_PAIR);
    lock = false;
}

// Prints the static ui and then prints the main menu.
void Screen::draw_menu(){
    print_ui_static();
    handle_opts();
}

void Screen::writelnattr_internals(char* str,int attr){
    while(lock);
    lock = true;
    time_t t;
    struct tm *ti;
    for (int n = logc - 1;n >= 0;n--){
	logv[n] = logv[n - 1];
	logattr[n] = logattr[n - 1];
    }
    time(&t);
    ti = localtime(&t);
    char *tstr = strdup(str);
    char buffer[4 * mc / 5];
    strftime(buffer,4 * mc / 5,"[%H:%M:%S]:",ti);
    strcat(buffer,tstr);
    logv[0] = strdup(buffer);
    logattr[0] = attr;
    free(tstr);
    fwrite(logv[0],sizeof(char),strlen(logv[0]),logf);
    fputc('\n',logf);
    for (int n = 0;n < logc;n++){
	move(mr - (n + 1),mc / 5 + 1);
	clrtoeol();
	attron(logattr[n]);
	mvprintw(mr - (n + 1),mc / 5 + 1,"%s",logv[n]);
	attroff(logattr[n]);
    }
    refresh();
    lock = false;
}

// Prints the basic UI features, they will be populated by the poll thread.
void Screen::print_ui_static(){
    while(lock);
    lock = true;
    attron(YELLOW_PAIR | A_BOLD);
    mvprintw(0,mc - 11,"NXT GROUP 9");
    mvprintw(3,0,"Battery:");
    mvprintw(5,0,"Motors:");
    attroff(A_BOLD);
    mvprintw(6,2,"Motor_1:");
    mvprintw(7,2,"Motor_2:");
    mvprintw(8,2,"Motor_3:");
    attron(A_BOLD);
    mvprintw(10,0,"Sensors:");
    attroff(A_BOLD);
    mvprintw(11,2,"Optical:");
    mvprintw(12,2,"Push_1:");
    mvprintw(13,2,"Push_2:");
    mvprintw(14,2,"RGB:");
    attron(A_BOLD);
    int c = mc / 5;
    for (int n = 2;n < mr;n++){
	mvprintw(n,c,"|");
    }
    for (int n = 0;n < mc;n++){
	mvprintw(1,n,"_");
    }
    attron(A_UNDERLINE | A_BOLD);
    mvprintw(1 ,0,"Robot Status:");
    mvprintw(1,c + 1,"Log:");
    attroff(A_UNDERLINE | A_BOLD);
    attroff(YELLOW_PAIR);
    lock = false;
}

// Prints the options and handles user input.
void Screen::handle_opts(){
    draw_stats();
    while(lock);
    lock = true;
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
    lock = false;
    refresh();
    int logc_temp,*logattr_temp;
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
	logattr_temp = (int*) malloc(sizeof(int) * logc);
	for (int n = 0;n < logc;n++){
	    logv_temp[n] = logv[n];
	    logattr_temp[n] = logattr[n];
	}
	free(logv);
	free(logattr);
	logv = logv_temp;
	logattr = logattr_temp;
	for (int n = logc_temp;n < logc;n++){
	    logv[n] = strdup("");
	    logattr[n] = 0;
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
	    while(lock);
	    attron(A_BOLD | YELLOW_PAIR);
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
	    attroff(A_BOLD | YELLOW_PAIR);
	    writelnattr("Starting remote control!",GREEN_PAIR);
	    r_remote(this);
	    return;
	    break;
	case 1:
	    writelnattr("Starting right side autonomous control",GREEN_PAIR);
	    //r_left(this);
	    //return;
	    break;
	case 2:
	    writelnattr("Starting left side autonomous control",GREEN_PAIR);
	    //r_right(this);
	    //return;
	    break;
	}
	break;
    }
    handle_opts();
}

// Sends a message to stay alive every minute, does not block the main thread.
// This was a bitch. uguu~~
void *stay_alive_tf(void *scr){
    char str[2] = {0x80,0x0D};
    while(1){
	((Screen*) scr)->nxt.sendBuffer(str,2);
	((Screen*) scr)->writelnattr("Sent STAY_ALIVE (0x0D) signal",
				     YELLOW_PAIR);
	sleep(60);
    }
}

void *log_tf(void *scr){
    while(1){
	if (!((Screen*) scr)->log.empty()){
	    ((Screen*) scr)->writelnattr_internals(
		((Screen*) scr)->log.front()->txt,
		((Screen*) scr)->log.front()->attr);
	    ((Screen*) scr)->log.pop();
	}
    }
}
