// Joe Jevnik
// 28.10.2013
// Remote control of the robot.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>

#include "../lestat/exceptions.h"
#include "../lestat/comms.h"
#include "../lestat/usbcomm.h"
#include "../lestat/bluecomm.h"
#include "../lestat/message.h"
#include "../lestat/opcodes.h"

#include "../screen/Screen.h"

using namespace std;

void r_remote(Screen *scr){
    while(1){
	switch(getch()){
	case 3:
	    return;
	    break;
	case 'p':
	    scr->op->setOutputState(0,0,2,2,0,0,false,NULL);
	    scr->op->setOutputState(1,0,2,2,0,0,false,NULL);
	    scr->op->setOutputState(2,0,2,2,0,0,false,NULL);
	    scr->m0 = 0;
	    scr->m1 = 0;
	    scr->m2 = 0;
	    clear();
	    scr->writelnattr("Exiting remote control!",COLOR_PAIR(1));
	    scr->draw_menu();
	    break;
	case ' ':
	    scr->op->setOutputState(0,0,2,2,0,0,false,NULL);
	    scr->op->setOutputState(1,0,2,2,0,0,false,NULL);
	    scr->op->setOutputState(2,0,2,2,0,0,false,NULL);
	    scr->m0 = 0;
	    scr->m1 = 0;
	    scr->m2 = 0;
	    scr->writeln("Stopping all motors");
	    scr->draw_stats();
	    break;
	case 'W':
		scr->m0 = 100;
		scr->op->setOutputState(0,scr->m0,2,2,0,0x20,0,false,NULL);
		scr->m1 = 100;
	        scr->op->setOutputState(1,scr->m1,2,2,0,0x20,0,false,NULL);
		scr->draw_stats();
		scr->writeln("Max foward speed");
	case 'w':
	    if (scr->m0 <= 90){
		scr->m0 += 10;
		scr->op->setOutputState(0,scr->m0,2,2,0,0x20,0,false,NULL);
		scr->draw_stats();
	    }
	    if (scr->m1 <= 90){
		scr->m1 += 10;
	        scr->op->setOutputState(1,scr->m1,2,2,0,0x20,0,false,NULL);
		scr->draw_stats();
	    }
	    scr->writeln("Increasing foward speed");
	    break;
	case 'S':
	    scr->m0 = -100;
	    scr->op->setOutputState(0,scr->m0,2,2,0,0x20,0,false,NULL);
	    scr->m1 = -100;
	    scr->op->setOutputState(1,scr->m1,2,2,0,0x20,0,false,NULL);
	    scr->draw_stats();
	    scr->writeln("Max reverse speed");
	case 's':
	    if (scr->m0 >= -90){
		scr->m0 -= 10;
		scr->op->setOutputState(0,scr->m0,2,2,0,0x20,0,false,NULL);
		scr->draw_stats();
	    }
	    if (scr->m1 >= -90){
		scr->m1 -= 10;
	        scr->op->setOutputState(1,scr->m1,2,2,0,0x20,0,false,NULL);
		scr->draw_stats();
	    }
	    scr->writeln("Increasing reverse speed");
	    break;
	case 'q':
	    if (scr->m0 <= 90){
		scr->m0 += 10;
		scr->op->setOutputState(0,scr->m0,2,2,0,0x20,0,false,NULL);
		scr->draw_stats();
		scr->writeln("Increasing foward left speed");
	    }
	    break;
	case 'e':
	    if (scr->m1 <= 90){
		scr->m1 += 10;
	        scr->op->setOutputState(1,scr->m1,2,2,0,0x20,0,false,NULL);
		scr->draw_stats();
		scr->writeln("Increasing foward right speed");
	    }	    
	    break;
	case 'a':
	    if (scr->m0 >= -90){
		scr->m0 -= 10;
		scr->op->setOutputState(0,scr->m0,2,2,0,0x20,0,false,NULL);
		scr->draw_stats();
		scr->writeln("Increasing reverse left speed");
	    }
	    break;
	case 'd':
	    if (scr->m1 >= -90){
		scr->m1 -= 10;
	        scr->op->setOutputState(1,scr->m1,2,2,0,0x20,0,false,NULL);
		scr->draw_stats();
		scr->writeln("Increasing reverse right speed");
	    }
	    break;
	case 'r':
	    scr->s0 = scr->op->getInputValues(0);
	    scr->s1 = scr->op->getInputValues(1);
	    scr->s2 = scr->op->getInputValues(2);
	    scr->s3 = scr->op->getInputValues(3);
	    scr->draw_stats();
	    scr->writeln("Requesting new sensor readings");
	}
    }
}
