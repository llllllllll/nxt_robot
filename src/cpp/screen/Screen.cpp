// Joe Jevnik
// 25.10.2013
// Implementation of Screen.

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
    logv = (struct msg_t*) malloc(logc * sizeof(struct msg_t));
    statw = newwin(12,6,3,10);
    ctlw = newwin(mr - 8,mc / 5,mr - 8,0);
    logw = newwin(logc,4 * mc / 5,2,mc / 5 + 1);
    for (int n = 0;n < logc;n++){
	logv[n].txt = strdup("");
	logv[n].attr = 0;
    }
    m0 = 0;
    m1 = 0;
    m2 = 0;
    lock = false;
    print_ui_static();
    writelnattr_internals("Starting session",DEFAULT_PAIR);
    writelnattr_internals("Press any key to connect...",A_BOLD);
    getch();
    writelnattr_internals("Attempting to connect...",DEFAULT_PAIR);
    try{
	nxt.connect(MAC_ADDRESS);
    } catch(NxtEx &ex){
        writelnattr_internals("ERROR: failed to connect!",RED_PAIR | A_BOLD);
        writelnattr_internals("Press any key to continue...",A_BOLD);
	getch();
	endwin();
	exit(1);
    }
    writelnattr_internals("Connection established!",GREEN_PAIR);
    writelnattr_internals("Grabbing initial sensor readings...",DEFAULT_PAIR);
    op = new Opcodes(&nxt);
    s0 = op->getInputValues(0);
    s1 = op->getInputValues(1);
    s2 = op->getInputValues(2);
    s3 = op->getInputValues(3);
    writelnattr_internals("Ready!",GREEN_PAIR | A_BOLD);
    op->setInputMode(0,LIGHT_ACTIVE,BOOLEANMODE,false,NULL);
    pthread_create(&stay_alive_thread,NULL,stay_alive_tf,(void*) this);
    pthread_create(&log_thread,NULL,log_tf,(void*) this);
    scr_refresh();
}

Screen::~Screen(){
    free(logv);
    fclose(logf);
    delete op;
    pthread_cancel(stay_alive_thread);
    pthread_cancel(log_thread);
    endwin();
}

void Screen::scr_refresh(){
    wrefresh(statw);
    wrefresh(ctlw);
    wrefresh(logw);
    refresh();
}

// Draws the robot stats.
void Screen::draw_stats(){
    while(lock);
    lock = true;
    unsigned short int b = op->getBatteryLevel();
    if (b > 500){ wattron(statw,GREEN_PAIR); }
    else { wattron(statw,RED_PAIR); }
    mvwprintw(statw,0,0,"%humV",op->getBatteryLevel());
    if (b > 500){ wattroff(statw,GREEN_PAIR); }
    else { wattroff(statw,RED_PAIR); }
    if (m0 >= 0){ wattron(statw,GREEN_PAIR); }
    else { wattron(statw,RED_PAIR); }
    mvwprintw(statw,3,0,"%+04d",m0);
    if (m0 > 0){ wattroff(statw,GREEN_PAIR); }
    else { wattroff(statw,RED_PAIR); }
    if (m1 >= 0){ wattron(statw,GREEN_PAIR); }
    else { wattron(statw,RED_PAIR); }
    mvwprintw(statw,4,0,"%+04d",m1);
    if (m1 >= 0){ wattroff(statw,GREEN_PAIR); }
    else { wattroff(statw,RED_PAIR); }
    if (m2 >= 0){ wattron(statw,GREEN_PAIR); }
    else { wattron(statw,RED_PAIR); }
    mvwprintw(statw,5,0,"%+04d",m2);
    if (m2 >= 0){ wattroff(statw,GREEN_PAIR); }
    else { wattroff(statw,RED_PAIR); }

    if (s0.calibratedValue > 500){ wattron(statw,GREEN_PAIR); }
    else { wattron(statw,RED_PAIR); }
    mvwprintw(statw,8,1,"%d",s0.calibratedValue);
    if (s0.calibratedValue > 500){ wattroff(statw,GREEN_PAIR); }
    else { wattroff(statw,RED_PAIR); }
    if (s1.calibratedValue > 500){ wattron(statw,GREEN_PAIR); }
    else { wattron(statw,RED_PAIR); }
    mvwprintw(statw,9,1,"%d",s1.calibratedValue);
    if (s1.calibratedValue > 500){ wattroff(statw,GREEN_PAIR); }
    else { wattroff(statw,RED_PAIR); }
    if (s2.calibratedValue > 500){ wattron(statw,GREEN_PAIR); }
    else { wattron(statw,RED_PAIR); }
    mvwprintw(statw,10,1,"%d",s2.calibratedValue);
    if (s2.calibratedValue > 500){ wattroff(statw,GREEN_PAIR); }
    else { wattroff(statw,RED_PAIR); }
    if (s3.calibratedValue > 500){ wattron(statw,GREEN_PAIR); }
    else { wattron(statw,RED_PAIR); }
    mvwprintw(statw,11,1,"%d",s3.calibratedValue);
    if (s3.calibratedValue > 500){ wattroff(statw,GREEN_PAIR); }
    else { wattroff(statw,RED_PAIR); }
    attroff(RED_PAIR);
    lock = false;
    scr_refresh();
}

// Prints the static ui and then prints the main menu.
void Screen::draw_menu(){
    wclear(ctlw);
    print_ui_static();
    draw_stats();
    handle_opts();
}

// Internals for printing to the console.
void Screen::writelnattr_internals(char *str,int attr){
    while(lock);
    lock = true;
    time_t t;
    struct tm *ti;
    free(logv[logc - 1].txt);
    for (int n = logc - 1;n >= 0;n--){
	logv[n] = logv[n - 1];
    }
    time(&t);
    ti = localtime(&t);
    char *buffer = (char*) malloc(4 * mc / 5 * sizeof(char));
    strftime(buffer,4 * mc / 5,"[%H:%M:%S]:",ti);
    strcat(buffer,str);
    logv[0].txt = buffer;
    logv[0].attr = attr;
    fwrite(logv[0].txt,sizeof(char),strlen(logv[0].txt),logf);
    fputc('\n',logf);
    for (int n = 0;n < logc;n++){
	wmove(logw,logc - (n + 1),0);
	wclrtoeol(logw);
	wattron(logw,logv[n].attr);
	mvwprintw(logw,logc - (n + 1),0,"%s",logv[n].txt);
	wattroff(logw,logv[n].attr);
    }
    scr_refresh();
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
    wattron(ctlw,A_BOLD);
    if (opt == 0){ wattron(ctlw,A_STANDOUT); }
    mvwprintw(ctlw,3,0,"REMOTE");
    if (opt == 0){ wattroff(ctlw,A_STANDOUT); }
    if (opt == 1){ wattron(ctlw,A_STANDOUT); }
    mvwprintw(ctlw,4,0,"RIGHT");
    if (opt == 1){ wattroff(ctlw,A_STANDOUT); }
    if (opt == 2){ wattron(ctlw,A_STANDOUT); }
    mvwprintw(ctlw,5,0,"LEFT");
    if (opt == 2){ wattroff(ctlw,A_STANDOUT); }
    wattroff(ctlw,A_BOLD);
    scr_refresh();
    int logc_temp;
    struct msg_t *logv_temp;
    switch(getch()){
    case 3:
	free(logv);
	pthread_cancel(log_thread);
	pthread_cancel(stay_alive_thread);
	endwin();
	exit(0);
	break;
    case KEY_RESIZE:
	endwin();
	getmaxyx(stdscr,mr,mc);
	logc_temp = logc;
	logc = 4 * mc / 5 - 1;
	logv_temp = (struct msg_t*) malloc(logc * sizeof(struct msg_t));
	for (int n = 0;n < logc;n++){
	    logv_temp[n] = logv[n];
	    free(logv[n].txt);
	}
	free(logv);
	logv = logv_temp;
	for (int n = logc_temp;n < logc;n++){
	    logv[n].txt = strdup("");
	    logv[n].attr = 0;
	}
	clear();
	scr_refresh();
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
	    wattron(ctlw,A_BOLD | YELLOW_PAIR);
	    wmove(ctlw,3,0);
	    wclrtoeol(ctlw);
	    wmove(ctlw,4,0);
	    wclrtoeol(ctlw);
	    wmove(ctlw,5,0);
	    wclrtoeol(ctlw);
	    mvwprintw(ctlw,0,0,"p - return");
	    mvwprintw(ctlw,1,0,"r - read sensors");
	    mvwprintw(ctlw,2,0,"SPACE - stop");
	    mvwprintw(ctlw,4,4,"w");
	    mvwprintw(ctlw,5,1,"q     e");
	    mvwprintw(ctlw,6,1,"a     d");
	    mvwprintw(ctlw,7,4,"s");
	    wattroff(ctlw,A_BOLD | YELLOW_PAIR);
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
	if (!((Screen*) scr)->log.empty() && !((Screen*) scr)->lock){
	    ((Screen*) scr)->writelnattr_internals(
		((Screen*) scr)->log.front()->txt,
		((Screen*) scr)->log.front()->attr);
	    ((Screen*) scr)->log.pop();
	}
	usleep(5000);
    }
}
