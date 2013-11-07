// Joe Jevnik
// 28.10.2013
// Remote control of the robot.

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>
#include <time.h>

#include "../C_NXT/nxt.h"

#include "../screen/scr.h"

void r_remote(SCR *scr,NXT *nxt){
    scr->m0->power = 0;
    scr->m0->mode = RUN_BRAKE;
    scr->m0->run_state = RUNNING;
    scr->m0->reg_mode = SYNCHRONIZATION;
    scr->m0->turn_ratio = 0;
    scr->m0->tacho_limit = 0;

    scr->m1->power = 0;
    scr->m1->mode = RUN_BRAKE;
    scr->m1->run_state = RUNNING;
    scr->m1->reg_mode = SYNCHRONIZATION;
    scr->m1->turn_ratio = 0;
    scr->m1->tacho_limit = 0;

    scr->m2->power = 0;
    scr->m2->mode = RUN_BRAKE;
    scr->m2->run_state = RUNNING;
    scr->m2->reg_mode = POWER_CONTROL;
    time_t t;
    while(1){
        switch(getch()){
	case 'f':
	    NXT_reset_motor_position(nxt,MOTOR_C,0,0,NULL);
	    scr->m2->power = 80;
	    scr->m2->run_state = RUNNING;
	    scr->m2->tacho_count = 0;
	    NXT_set_motorstate(nxt,scr->m2,0,NULL);
	    while(scr->m2->tacho_count < 30){
		free(scr->m2);
		scr->m2 = NXT_get_motorstate(nxt,MOTOR_C);
	    }
	    scr->m2->power = 0;
	    scr->m2->run_state = IDLE;
	    NXT_set_motorstate(nxt,scr->m2,0,NULL);
	    SCR_writeln(scr,"Dispensing");
	    break;
	case 'p':
	    scr->m0->power = 0;
	    scr->m1->power = 0;
	    NXT_set_motorstate(nxt,scr->m0,0,NULL);
	    NXT_set_motorstate(nxt,scr->m1,0,NULL);
	    SCR_draw_stats(scr);
	    SCR_writelnattr(scr,"Exiting remote control.",GREEN_PAIR);
	    SCR_draw_menu(scr);
	case ' ':
	    scr->m0->power = 0;
	    scr->m1->power = 0;
	    scr->m2->power = 0;
	    NXT_set_motorstate(nxt,scr->m0,0,NULL);
	    NXT_set_motorstate(nxt,scr->m1,0,NULL);
	    NXT_set_motorstate(nxt,scr->m2,0,NULL);
	    SCR_draw_stats(scr);
	    SCR_writeln(scr,"Stopping all motors");
	    break;
	case 'W':
	    scr->m0->power = 100;
	    scr->m1->power = 100;
	    NXT_set_motorstate(nxt,scr->m0,0,NULL);
	    NXT_set_motorstate(nxt,scr->m1,0,NULL);
	    SCR_writeln(scr,"Max foward speed");
	case 'w':
	    if (scr->m0->power <= 90){
		scr->m0->power += 10;
		NXT_set_motorstate(nxt,scr->m0,0,NULL);
	    }
	    if (scr->m1->power <= 90){
		scr->m1->power += 10;
		NXT_set_motorstate(nxt,scr->m1,0,NULL);
	    }
	    SCR_draw_stats(scr);
	    SCR_writeln(scr,"Increasing foward speed");
	    break;
	case 'S':
	    scr->m0->power = -100;
	    scr->m1->power = -100;
	    NXT_set_motorstate(nxt,scr->m0,0,NULL);
	    NXT_set_motorstate(nxt,scr->m1,0,NULL);
	    SCR_writeln(scr,"Max reverse speed");
	    break;
	case 's':
	    if (scr->m0->power >= -90){
		scr->m0->power -= 10;
		NXT_set_motorstate(nxt,scr->m0,0,NULL);
	    }
	    if (scr->m1->power >= -90){
		scr->m1->power -= 10;
		NXT_set_motorstate(nxt,scr->m1,0,NULL);	    
		SCR_draw_stats(scr);
	    }
	    SCR_writeln(scr,"Increasing reverse speed");
	    break;
	case 'q':
	    if (scr->m0->power <= 90){
		scr->m0->power += 10;
		NXT_set_motorstate(nxt,scr->m0,0,NULL);
		SCR_draw_stats(scr);
		SCR_writeln(scr,"Increasing foward left speed");
	    }
	    break;
	case 'a':
	    if (scr->m0->power >= -90){
		scr->m0->power -= 10;
		NXT_set_motorstate(nxt,scr->m0,0,NULL);
		SCR_draw_stats(scr);
		SCR_writeln(scr,"Decreasing foward left speed");
	  }
	    break;
	case 'e':
	    if (scr->m1->power <= 90){
		scr->m1->power += 10;
		NXT_set_motorstate(nxt,scr->m1,0,NULL);
		SCR_draw_stats(scr);
		SCR_writeln(scr,"Increasing foward right speed");
	    }
	    break;
	case 'd':
	    if (scr->m1->power >= -90){
		scr->m1->power -= 10;
		NXT_set_motorstate(nxt,scr->m1,0,NULL);
		SCR_draw_stats(scr);
		SCR_writeln(scr,"Increasing reverse right speed");
	    }
	    break;
	case 'r':
	    free(scr->s0);
	    free(scr->s1);
	    free(scr->s2);
	    free(scr->s3);
	    scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
	    scr->s1 = NXT_get_sensorstate(nxt,SENSOR_2);
	    scr->s2 = NXT_get_sensorstate(nxt,SENSOR_3);
	    scr->s3 = NXT_get_sensorstate(nxt,SENSOR_4);
	    SCR_draw_stats(scr);
	    SCR_writeln(scr,"Requesting new sensor readings");
	    break;
	case 3:
	    scr->m0->power = 0;
	    scr->m1->power = 0;
	    NXT_set_motorstate(nxt,scr->m0,0,NULL);
	    NXT_set_motorstate(nxt,scr->m1,0,NULL);
	    SCR_writelnattr(scr,"Exiting remote control!",COLOR_PAIR(1));
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
