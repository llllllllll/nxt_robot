#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>
#include <time.h>

#include "../C_NXT/nxt.h"
#include "r_logic.h"
#include "../screen/scr.h"
 
void start_timer(SCR *scr,int seconds,int *f){
    pthread_t t;
    struct timer_flag *tf = malloc(sizeof(struct timer_flag));
    tf->scr = scr;
    tf->secs = seconds;
    tf->flag = f;
    pthread_create(&t,NULL,g_start_timer,tf);
}
 
void* g_start_timer(void *args){
    int seconds = ((struct timer_flag*) args)->secs;
    int *flag   = ((struct timer_flag*) args)->flag;
    SCR *scr    = ((struct timer_flag*) args)->scr;
    _current_time = clock() + seconds * CLOCKS_PER_SEC;
    while(clock() < _current_time);
    *flag = 1;
    SCR_writelnattr(scr,"TIMER DONE! push sensor now active!",YELLOW_PAIR);
    free(args);
    pthread_exit(NULL);
}

void r_right(SCR *scr,NXT *nxt){
    time_t t;
    int running = 1, dispensing = 0;
    start_timer(scr,25,&dispensing);

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

    scr->m0->power = 100;
    scr->m1->power = 100;
    free(scr->s0);
    scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
    NXT_set_motorstate(nxt,scr->m0,0,NULL);
    NXT_set_motorstate(nxt,scr->m1,0,NULL);
    while(scr->s0->calibrated_value < 500){
	free(scr->s0);
	scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
	SCR_draw_stats(scr);
    }

    while(running){
	scr->m0->power = -30;
	scr->m1->power = 80;
	NXT_set_motorstate(nxt,scr->m0,0,NULL);
	NXT_set_motorstate(nxt,scr->m1,0,NULL);
	while (scr->s0->calibrated_value < 500){
	    free(scr->s0);
	    scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
	    SCR_draw_stats(scr);
	}
	SCR_writeln(scr,"Switching to BLACK");
	scr->m0->power = 80;
	scr->m1->power = -30;
	NXT_set_motorstate(nxt,scr->m0,0,NULL);
	NXT_set_motorstate(nxt,scr->m1,0,NULL);
	while (scr->s0->calibrated_value >= 500){
	    free(scr->s0);
	    scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
	    SCR_draw_stats(scr);
	    if (dispensing){
		free(scr->s1);
		scr->s1 = NXT_get_sensorstate(nxt,SENSOR_2);
		if (scr->s1->raw_value < 500){
		    NXT_reset_motor_position(nxt,MOTOR_C,0,0,NULL);
		    scr->m2->power = 80;
		    scr->m2->run_state = RUNNING;
		    scr->m2->tacho_count = 0;
		    NXT_set_motorstate(nxt,scr->m2,0,NULL);
		    while(scr->m2->tacho_count < 40){
			free(scr->m2);
			scr->m2 = NXT_get_motorstate(nxt,MOTOR_C);
		    }
		    scr->m2->power = 0;
		    scr->m2->run_state = IDLE;
		    NXT_set_motorstate(nxt,scr->m2,0,NULL);
		    SCR_writeln(scr,"Dispensing");
		    running = 0;
		    break;
		}
	    }
	}
	SCR_writeln(scr,"Switching to WHITE!");
    }
    scr->m0->power = 80;
    scr->m1->power = -80;
    NXT_set_motorstate(nxt,scr->m0,0,NULL);
    NXT_set_motorstate(nxt,scr->m1,0,NULL);
    while (scr->s0->calibrated_value < 500){
	free(scr->s0);
	scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
	SCR_draw_stats(scr);
	if (dispensing){
	    free(scr->s1);
	    scr->s1 = NXT_get_sensorstate(nxt,SENSOR_2);
	    if (scr->s1->raw_value < 500){
		NXT_reset_motor_position(nxt,MOTOR_C,0,0,NULL);
		scr->m2->power = 80;
		scr->m2->run_state = RUNNING;
		scr->m2->tacho_count = 0;
		NXT_set_motorstate(nxt,scr->m2,0,NULL);
		while(scr->m2->tacho_count < 40){
		    free(scr->m2);
		    scr->m2 = NXT_get_motorstate(nxt,MOTOR_C);
		}
		scr->m2->power = 0;
		scr->m2->run_state = IDLE;
		NXT_set_motorstate(nxt,scr->m2,0,NULL);
		SCR_writeln(scr,"Dispensing");
		running = 0;
		break;
	    }
	}
    }
    running = 1;
    SCR_writeln(scr,"BACKING UP!");
    scr->m0->power = -100;
    scr->m1->power = -100;
    NXT_set_motorstate(nxt,scr->m0,0,NULL);
    NXT_set_motorstate(nxt,scr->m1,0,NULL);
    time(&t);
    while(time(NULL) - t < 2);
    SCR_writeln(scr,"TURNING!");
    scr->m0->power = 100;
    scr->m1->power = -100;
    NXT_set_motorstate(nxt,scr->m0,0,NULL);
    NXT_set_motorstate(nxt,scr->m1,0,NULL);
    while (scr->s0->calibrated_value <= 500){
	free(scr->s0);
	scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
	SCR_draw_stats(scr);
    }
    while(running){
	scr->m0->power = -30;
	scr->m1->power = 80;
	NXT_set_motorstate(nxt,scr->m0,0,NULL);
	NXT_set_motorstate(nxt,scr->m1,0,NULL);
	while (scr->s0->calibrated_value >= 500){
	    free(scr->s0);
	    scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
	    SCR_draw_stats(scr);
	}
	SCR_writeln(scr,"Switching to WHITE");
	scr->m0->power = 80;
	scr->m1->power = -30;
	NXT_set_motorstate(nxt,scr->m0,0,NULL);
	NXT_set_motorstate(nxt,scr->m1,0,NULL);
	while (scr->s0->calibrated_value < 500){
	    free(scr->s0);
	    scr->s0 = NXT_get_sensorstate(nxt,SENSOR_1);
	    SCR_draw_stats(scr);
	}
	SCR_writeln(scr,"Switching to BLACK!");
    }
    scr->m0->power = 0;
    scr->m1->power = 0;
    NXT_set_motorstate(nxt,scr->m0,0,NULL);
    NXT_set_motorstate(nxt,scr->m1,0,NULL);
}
