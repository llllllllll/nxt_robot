// Joe Jevnik
// 25.10.2013
// Implementation of Screen.

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sched.h>
#include <unistd.h>

#include "scr.h"
#include "../robot_control/r_logic.h"

SCR *alloc_SCR(NXT *nxt){
    SCR *scr = malloc(sizeof(SCR));
    initscr();
    getmaxyx(stdscr,scr->mr,scr->mc);
    start_color();
    use_default_colors();
    init_pair(1,COLOR_GREEN,-1);
    init_pair(2,COLOR_RED,-1);
    init_pair(3,COLOR_YELLOW,-1);
    init_pair(4,COLOR_MAGENTA,-1);
    raw();
    keypad(stdscr,TRUE);
    curs_set(0);
    noecho();
    scr->nxt = nxt;
    scr->opt = 0;
    scr->logf = fopen(path_to_logfile,"a");
    assert(scr->logf);
    scr->logc = scr->mr - 2;
    scr->logv = malloc(scr->logc * sizeof(logmsg_t));
    scr->statw = newwin(12,6,3,10);
    scr->ctlw = newwin(scr->mr - 8,scr->mc / 5,scr->mr - 8,0);
    scr->logw = newwin(scr->logc,4 * scr->mc / 5,2,scr->mc / 5 + 1);
    for (int n = 0;n < scr->logc;n++){
	scr->logv[n].txt = strdup("");
	scr->logv[n].attr = 0;
    }
    SCR_printui_static(scr);
    SCR_writelnattr(scr,"Starting session",DEFAULT_PAIR);
    SCR_writelnattr(scr,"Press any key to connect...",A_BOLD);
    getch();
    SCR_writelnattr(scr,"Attempting to connect...",DEFAULT_PAIR);
    if(NXT_connect(nxt,MAC_ADDRESS)){
        SCR_writelnattr(scr,"ERROR: failed to connect!",
				  RED_PAIR | A_BOLD);
        SCR_writelnattr(scr,"Press any key to continue...",A_BOLD);
	getch();
	endwin();
	free(scr);
	return NULL;
    }
    scr->m0 = NXT_get_motorstate(nxt,MOTOR_A);
    scr->m1 = NXT_get_motorstate(nxt,MOTOR_B);
    scr->m2 = NXT_get_motorstate(nxt,MOTOR_C);
    SCR_writelnattr(scr,"Connection established!",GREEN_PAIR);
    SCR_writelnattr(scr,"Grabbing initial sensor readings...",
			      DEFAULT_PAIR);
    scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
    scr->s1 = NXT_get_sensorstate(nxt,SENSOR_2);
    scr->s2 = NXT_get_sensorstate(nxt,SENSOR_3);
    scr->s3 = NXT_get_sensorstate(nxt,SENSOR_4);
    NXT_set_input_mode(nxt,0,LIGHT_ACTIVE,BOOLEAN_MODE,0,NULL);
    SCR_writelnattr(scr,"Ready!",GREEN_PAIR | A_BOLD);
    pthread_create(&scr->stay_alive_thread,NULL,stay_alive_tf,scr);
    SCR_refresh(scr);
    return scr;
}

void free_SCR(SCR *scr){
    free(scr->logv);
    fclose(scr->logf);
    free(scr->m0);
    free(scr->m1);
    free(scr->m2);
    echo();
    endwin();
}

void SCR_refresh(SCR *scr){
    wrefresh(scr->statw);
    wrefresh(scr->ctlw);
    wrefresh(scr->logw);
    refresh();
}

// Draws the robot stats.
void SCR_draw_stats(SCR *scr){
    unsigned short int b = NXT_battery_level(scr->nxt);
    if (b > 6000){ wattron(scr->statw,GREEN_PAIR); }
    else { wattron(scr->statw,RED_PAIR); }
    mvwprintw(scr->statw,0,0,"%humV",NXT_battery_level(scr->nxt));
    if (b > 6000){ wattroff(scr->statw,GREEN_PAIR); }
    else { wattroff(scr->statw,RED_PAIR); }

    if (scr->m0->power >= 0){ wattron(scr->statw,GREEN_PAIR); }
    else { wattron(scr->statw,RED_PAIR); }
    mvwprintw(scr->statw,3,1,"%+04d",scr->m0->power);
    if (scr->m0->power > 0){ wattroff(scr->statw,GREEN_PAIR); }
    else { wattroff(scr->statw,RED_PAIR); }
    if (scr->m1->power >= 0){ wattron(scr->statw,GREEN_PAIR); }
    else { wattron(scr->statw,RED_PAIR); }
    mvwprintw(scr->statw,4,1,"%+04d",scr->m1->power);
    if (scr->m1->power >= 0){ wattroff(scr->statw,GREEN_PAIR); }
    else { wattroff(scr->statw,RED_PAIR); }
    if (scr->m2->power >= 0){ wattron(scr->statw,GREEN_PAIR); }
    else { wattron(scr->statw,RED_PAIR); }
    mvwprintw(scr->statw,5,1,"%+04d",scr->m2->power);
    if (scr->m2->power >= 0){ wattroff(scr->statw,GREEN_PAIR); }
    else { wattroff(scr->statw,RED_PAIR); }

    if (scr->s0->calibrated_value > 500){ wattron(scr->statw,GREEN_PAIR); }
    else { wattron(scr->statw,RED_PAIR); }
    mvwprintw(scr->statw,8,1,"%d",scr->s0->calibrated_value);
    if (scr->s0->calibrated_value > 500){ wattroff(scr->statw,GREEN_PAIR); }
    else { wattroff(scr->statw,RED_PAIR); }
    if (scr->s1->calibrated_value > 500){ wattron(scr->statw,GREEN_PAIR); }
    else { wattron(scr->statw,RED_PAIR); }
    mvwprintw(scr->statw,9,1,"%d",scr->s1->calibrated_value);
    if (scr->s1->calibrated_value > 500){ wattroff(scr->statw,GREEN_PAIR); }
    else { wattroff(scr->statw,RED_PAIR); }
    if (scr->s2->calibrated_value > 500){ wattron(scr->statw,GREEN_PAIR); }
    else { wattron(scr->statw,RED_PAIR); }
    mvwprintw(scr->statw,10,1,"%d",scr->s2->calibrated_value);
    if (scr->s2->calibrated_value > 500){ wattroff(scr->statw,GREEN_PAIR); }
    else { wattroff(scr->statw,RED_PAIR); }
    if (scr->s3->calibrated_value > 500){ wattron(scr->statw,GREEN_PAIR); }
    else { wattron(scr->statw,RED_PAIR); }
    mvwprintw(scr->statw,11,1,"%d",scr->s3->calibrated_value);
    if (scr->s3->calibrated_value > 500) { wattroff(scr->statw,GREEN_PAIR); }
    else { wattroff(scr->statw,RED_PAIR); }
    SCR_refresh(scr);
}

// Prints the static ui and then prints the main menu.
void SCR_draw_menu(SCR *scr){
    wclear(scr->ctlw);
    SCR_draw_stats(scr);
    SCR_handle_opts(scr);
}

// Internals for printing to the console.
void SCR_writelnattr(SCR *scr,char *str,int attr){
    time_t t;
    struct tm *ti;
    free(scr->logv[scr->logc - 1].txt);
    for (int n = scr->logc - 1;n >= 0;n--){
	scr->logv[n] = scr->logv[n - 1];
    }
    time(&t);
    ti = localtime(&t);
    char *buffer = malloc(4 * scr->mc / 5 * sizeof(char));
    strftime(buffer,4 * scr->mc / 5,"[%H:%M:%S]:",ti);
    strcat(buffer,str);
    scr->logv[0].txt = buffer;
    scr->logv[0].attr = attr;
    fwrite(scr->logv[0].txt,sizeof(char),strlen(scr->logv[0].txt),scr->logf);
    fputc('\n',scr->logf);
    for (int n = 0;n < scr->logc;n++){
	wmove(scr->logw,scr->logc - (n + 1),0);
	wclrtoeol(scr->logw);
	wattron(scr->logw,scr->logv[n].attr);
	mvwprintw(scr->logw,scr->logc - (n + 1),0,"%s",scr->logv[n].txt);
	wattroff(scr->logw,scr->logv[n].attr);
    }
    SCR_refresh(scr);
}

// Prints the basic UI features, they will be populated by the poll thread.
void SCR_printui_static(SCR *scr){
    attron(YELLOW_PAIR | A_BOLD);
    mvprintw(0,scr->mc - 11,"NXT GROUP 9");
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
    int c = scr->mc / 5;
    for (int n = 2;n < scr->mr;n++){
	mvprintw(n,c,"|");
    }
    for (int n = 0;n < scr->mc;n++){
	mvprintw(1,n,"_");
    }
    attron(A_UNDERLINE | A_BOLD);
    mvprintw(1 ,0,"Robot Status:");
    mvprintw(1,c + 1,"Log:");
    attroff(A_UNDERLINE | A_BOLD);
    attroff(YELLOW_PAIR);
}

// Resizes the screen to fit a newly resized window.
// TODO. FIX resizing.
void SCR_handle_resize(SCR *scr){
    int logc_temp;
    logmsg_t *logv_temp;
    endwin();
    getmaxyx(stdscr,scr->mr,scr->mc);
    logc_temp = scr->logc;
    scr->logc = scr->mr - 2;
    logv_temp = realloc(scr->logv,scr->logc * sizeof(logmsg_t));
    scr->logv = logv_temp;
    wclear(scr->ctlw);
    wclear(scr->logw);
    delwin(scr->ctlw);
    delwin(scr->logw);
    scr->ctlw = newwin(scr->mr - 8,scr->mc / 5,scr->mr - 8,0);
    scr->logw = newwin(scr->logc,4 * scr->mc / 5,2,scr->mc / 5 + 1);
    for (int n = logc_temp;n < scr->logc;n++){
	scr->logv[n].txt = strdup("");
	scr->logv[n].attr = 0;
    }
    SCR_refresh(scr);
    clear();
    SCR_draw_menu(scr);
    SCR_refresh(scr);
}

// Prints the options and handles user input.
void SCR_handle_opts(SCR *scr){
    SCR_draw_stats(scr);
    wattron(scr->ctlw,A_BOLD);
    if (scr->opt == 0){ wattron(scr->ctlw,A_STANDOUT); }
    mvwprintw(scr->ctlw,3,0,"REMOTE");
    if (scr->opt == 0){ wattroff(scr->ctlw,A_STANDOUT); }
    if (scr->opt == 1){ wattron(scr->ctlw,A_STANDOUT); }
    mvwprintw(scr->ctlw,4,0,"RIGHT");
    if (scr->opt == 1){ wattroff(scr->ctlw,A_STANDOUT); }
    if (scr->opt == 2){ wattron(scr->ctlw,A_STANDOUT); }
    mvwprintw(scr->ctlw,5,0,"LEFT");
    if (scr->opt == 2){ wattroff(scr->ctlw,A_STANDOUT); }
    wattroff(scr->ctlw,A_BOLD);
    SCR_refresh(scr);
    switch(getch()){
    case 3:
	free_SCR(scr);
	return;
	break;
    case KEY_RESIZE:
	SCR_handle_resize(scr);
	return;
	break;
    case KEY_UP:
	if (scr->opt > 0){
	    --scr->opt;
	}
	break;
    case KEY_DOWN:
	if (scr->opt < 2){
	    ++scr->opt;
	}
	break;
    case 10: // ENTER
	switch(scr->opt){
	case 0:
	    wattron(scr->ctlw,A_BOLD);
	    wmove(scr->ctlw,3,0);
	    wclrtoeol(scr->ctlw);
	    wmove(scr->ctlw,4,0);
	    wclrtoeol(scr->ctlw);
	    wmove(scr->ctlw,5,0);
	    wclrtoeol(scr->ctlw);
	    mvwprintw(scr->ctlw,0,0,"p - return");
	    mvwprintw(scr->ctlw,1,0,"r - read sensors");
	    mvwprintw(scr->ctlw,2,0,"SPACE - stop");
	    mvwprintw(scr->ctlw,4,4,"w");
	    mvwprintw(scr->ctlw,5,1,"q     e");
	    mvwprintw(scr->ctlw,6,1,"a     d");
	    mvwprintw(scr->ctlw,7,4,"s");
	    wattroff(scr->ctlw,A_BOLD);
	    SCR_writelnattr(scr,"Starting remote control!",GREEN_PAIR);
	    r_remote(scr,scr->nxt);
	    return;
	    break;
	case 1:
	    SCR_writelnattr(scr,"Starting right side autonomous control",
			    GREEN_PAIR);
	    //r_left(scr);
	    //return;
	    break;
	case 2:
	    SCR_writelnattr(scr,"Starting left side autonomous control",
			    GREEN_PAIR);
	    //r_right(this);
	    //return;
	    break;
	}
	break;
    }
    SCR_handle_opts(scr);
}

// Sends a message to stay alive every minute, does not block the main thread.
void *stay_alive_tf(void *scr){
    while(1){
	NXT_stay_alive(((SCR*) scr)->nxt);
	SCR_writelnattr((SCR*) scr,"Sent STAY_ALIVE (0x0D) message",YELLOW_PAIR);
	sleep(60);
    }
}
