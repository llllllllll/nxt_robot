// Joe Jevnik
// 28.10.2013
// Remote control of the robot.

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>

#include "../C_NXT/nxt.h"

#include "../screen/scr.h"

void r_remote(SCR *scr,NXT *nxt){
    motorstate_t *tmp0 = malloc(sizeof(motorstate_t));
    motorstate_t *tmp1 = malloc(sizeof(motorstate_t));
    while(1){
        switch(getch()){
	case 'w':
	    tmp0->port = 0;
	    tmp0->power = 100;
	    tmp0->mode = 0x02;
	    tmp0->run_state = 2;
	    tmp0->reg_mode = 2;
	    tmp0->run_state = 0x20;
	    tmp0->turn_ratio = 0;
	    tmp0->tacho_limit = 0;

	    tmp1->port = 1;
	    tmp1->power = 100;
	    tmp1->mode = 0x02;
	    tmp1->run_state = 2;
	    tmp1->reg_mode = 2;
	    tmp1->run_state = 0x20;
	    tmp1->turn_ratio = 0;
	    tmp1->tacho_limit = 0;
	    NXT_set_motorstate(nxt,tmp0,0,NULL);
	    NXT_set_motorstate(nxt,tmp1,0,NULL);
	    SCR_writeln(scr,"move foward!!");
	    break;
	case 's':
	    tmp0->power = 0;
	    tmp1->power = 0;
	    NXT_set_motorstate(nxt,tmp0,0,NULL);
	    NXT_set_motorstate(nxt,tmp1,0,NULL);
	    break;
	case 3:
	    tmp0->power = 0;
	    tmp1->power = 0;
	    NXT_set_motorstate(nxt,tmp0,0,NULL);
	    NXT_set_motorstate(nxt,tmp1,0,NULL);
	    free(tmp0);
	    free(tmp1);
	    free_SCR(scr);
	    exit(0);
	    return;
	}
    }
}

/*
    0,
    scr->m0,
    2,
    2,
    0,
    0x20,
    0,
    false,
    NULL

    unsigned char port;
    char power;
    unsigned char mode;
    unsigned char reg_mode;
    char turn_ratio;
    unsigned char run_state;
    unsigned int tacho_limit;
    int tacho_count;
    int block_tacho_count;
    int rotation_count;
*/
