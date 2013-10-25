

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "exceptions.h"
#include "comms.h"
#include "usbcomm.h"
#include "bluecomm.h"
#include "message.h"
#include "opcodes.h"

using namespace std;

void printBatteryLevel(Comms &nxt){
    Opcodes op(&nxt);
    printf("Battery level: %hu [mv]\n",op.getBatteryLevel());
}

int main(int argc, char *argv[]){
    BlueComm nxt;
    try{
        nxt.connect("00:16:53:1A:14:6A");
    } catch(NxtEx &ex){
        cout << ex.toString() << endl;
        return EXIT_FAILURE;
    }

    try{
        Opcodes op(&nxt);
        unsigned char m1_status,m2_status,m3_status,test_char;
	MotorState m1_state,m2_state,m3_state;
	SensorState s1,s2,s3,s4;
        //op.setOutputState(0x01,80,0x01,0x00,50,0x20,980,true,&status);
        sleep(1);
        //op.setOutputState(0x02,80,0x01,0x00,50,0x20,980,true,&status);;
        op.setInputMode(0x00, LIGHT_ACTIVE, RAWMODE);

        cout << "Status: "<< (int) m1_status << endl;
        sleep(1);
       

	while (1){
	    cin >> test_char;
	    switch(test_char){
	    case 'q':
		op.resetMotorPosition(0x02,false,true,&m3_status);
		op.setOutputState(0x02,100,0x02,0x02,0,0x20,300,true,&m3_status);
		m1_state = op.getOutputState(0x03);
		while (m1_state.tachoCount < 80){
		    m1_state = op.getOutputState(0x02);
		}
		op.setOutputState(0x02,0,0x02,0x02,0,0x20,0,true,&m3_status);
		break;
	    case 'w':
		s1 = op.getInputValues(0x00);
		op.setOutputState(0x00,100,0x02,0x02,0,0x20,0,true,&m1_status);
		op.setOutputState(0x01,100,0x02,0x02,0,0x20,0,true,&m2_status);
		while (s1.calibratedValue < 400){
		    cout << "p1: " << s1.calibratedValue << endl;
		    s1 = op.getInputValues(0x00);
		}
		cout << "p1 tripped on corner 1!" << endl;
		op.setOutputState(0x00,-100,0x02,0x02,0,0x20,0,true,&m1_status);
		op.setOutputState(0x01,-100,0x02,0x02,0,0x20,0,true,&m2_status);
		while (s1.calibratedValue > 400){
		    cout << "p1: " << s1.calibratedValue << endl;
		    s1 = op.getInputValues(0x00);
		}
	        op.setOutputState(0x00,-50,0x04,0x02,0,0x20,0,true,&m1_status);
		op.setOutputState(0x01,100,0x04,0x02,0,0x20,0,true,&m2_status);
		break;
	    case 'a':
		op.setOutputState(0x00,0,0x04,0x02,0,0x20,0,true,&m1_status);
		op.setOutputState(0x01,100,0x04,0x02,0,0x20,0,true,&m2_status);
		break;
	    case 's':
	        op.setOutputState(0x00,0,0x00,0x00,50,0x00,720,false,&m1_status);
		op.setOutputState(0x01,0,0x00,0x00,50,0x00,720,false,&m2_status);
		op.setOutputState(0x02,0,0x00,0x00,50,0x00,720,false,&m2_status);
		break;
	    }
	    s1 = op.getInputValues(0x00);
	    s2 = op.getInputValues(0x01);
	    s3 = op.getInputValues(0x02);
	    s4 = op.getInputValues(0x03);
	    cout << "p1: " << s1.calibratedValue << endl;
	    cout << "p2: " << s2.calibratedValue << endl;
	    cout << "p3: " << s3.normalizedValue << endl;
	    cout << "p4: " << s4.calibratedValue << endl;
	}
	op.setOutputState(0x00,0,0x00,0x00,50,0x00,720,false,&m1_status);

        nxt.disconnect();

    }catch(NxtEx &ex){
        cout << ex.toString() << endl;
        nxt.disconnect();
        return EXIT_FAILURE;

    }
    return EXIT_SUCCESS;
}
