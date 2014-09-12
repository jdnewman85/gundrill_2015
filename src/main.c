//GunDrill

//TODO Should have a function to handle drive status for every function that can return it. If it returns bad, abort
//TODO Reset for drive
//TODO Error better
//TODO Feedhold?
//TODO MIN MAX LIMIT

#include <stdlib.h> //abs()
#include <time.h>
#include <stdio.h>
#include <signal.h>

#include <gtk/gtk.h>

#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/simplemotion_private.h"
#include "SimpleMotion/vsd_cmd.h"
#include "SimpleMotion/vsd_drive_bits.h"

#include "axis.h"
#include "gui.h"
#include "aux.h"
#include "event.h"
#include "io.h"

#include "constant.h"
#include "main.h"

int Position;
int Target;
int Feedrate;
int FeedrateOverride;
int Spindle;
int SpindleOverride;
char Status[STATUS_LENGTH];

int InputType;

float CntPerInch = CNT_PER_REV * REV_PER_INCH;
float VelPerRPM = VEL_PER_RPM;
float VelPerIPM = REV_PER_INCH*VEL_PER_RPM;

int State = STATE_STARTUP;

int JogFeedrate[] = {JOG_FEEDRATE_1X, JOG_FEEDRATE_10X, JOG_FEEDRATE_100X};

int JogMode = JOG_MODE_DEFAULT;
int JogDirection = JOG_STOP;

char* StatusText = "";
char* ErrorText = "";

void doState() {
	smint32 simpleStatus;

	switch(State) {
	case STATE_STARTUP:
		StatusText = "Startup";
		break;
	case STATE_IDLE:
		setOutput(OUTPUT_FEED_FORWARD, 0);
		setOutput(OUTPUT_CYCLE_START, 0);
		setOutput(OUTPUT_RAPID_RETRACT, 0);
		StatusText = "";
		break;
	case STATE_START:
		//Start part, and set into feed
		smSetParam(AxisName, "VelocityLimit", (int)withOverride(Feedrate, FeedrateOverride, FEEDRATE_MIN, FEEDRATE_MAX));
		//Set IO
		setOutput(OUTPUT_CYCLE_START, 1);
		setOutput(OUTPUT_FEED_FORWARD, 1);
		//Reset 0 to current location
		smSetParam(AxisName, "ControlMode", CONTROL_MODE_VELOCITY);
		smSetParam(AxisName, "ControlMode", CONTROL_MODE_POSITION);
		Position = 0;
		previousDrivePosition = 0;
		diff = 0;
		//Move
		AxisStatus = smCommand(AxisName, "ABSTARGET", Target);
		StatusText = "Feeding";
		State = STATE_FEED;
		break;
	case STATE_FEED:
		//Feeding, wait for move complete, then return
		//Update Feedrate incase override has been changed
		smSetParam(AxisName, "VelocityLimit", withOverride(Feedrate, FeedrateOverride, FEEDRATE_MIN, FEEDRATE_MAX));
		//Move Complete?
		AxisStatus = smGetParam(AxisName, "SimpleStatus", &simpleStatus);
		if(SIMPLE_STATUS_IDLE == simpleStatus) {
			//Set IO
			setOutput(OUTPUT_FEED_FORWARD, 0);
			setOutput(OUTPUT_CYCLE_START, 0);
			setOutput(OUTPUT_RAPID_RETRACT, 1);
			//Rapid back
			smSetParam(AxisName, "VelocityLimit", FEEDRATE_RAPID);
			AxisStatus = smCommand(AxisName, "ABSTARGET", -BACKLASH_MOVE_AMOUNT);
			StatusText = "Returning";
			State = STATE_RETURN;
		}
		break;
	case STATE_RETURN:
		//Returning, wait for move complete, then return to 0
		//Move Complete?
		AxisStatus = smGetParam(AxisName, "SimpleStatus", &simpleStatus);
		if(SIMPLE_STATUS_IDLE == simpleStatus) {
			//Complete
			AxisStatus = smCommand(AxisName, "ABSTARGET", 0);
			StatusText = "Backlash Comp";
			State = STATE_RETURN_ZERO;
		}
		break;
	case STATE_RETURN_ZERO:
		//Returning to 0, then idle
		//Move Complete?
		AxisStatus = smGetParam(AxisName, "SimpleStatus", &simpleStatus);
		if(SIMPLE_STATUS_IDLE == simpleStatus) {
			//Complete
			//TODO Parts counter?
			//Set IO
			setOutput(OUTPUT_RAPID_RETRACT, 1);
			if(!ErrorText) {
				State = STATE_IDLE;
			}else {
				State = STATE_ERROR;
			}
		}
		break;
	case STATE_STOP:
		smSetParam(AxisName, "VelocityLimit", FEEDRATE_MAX);
		smSetParam(AxisName, "ControlMode", CONTROL_MODE_VELOCITY);
		AxisStatus = smCommand(AxisName, "ABSTARGET", 0);
		smSetParam(AxisName, "ControlMode", CONTROL_MODE_POSITION);
		State = STATE_IDLE;
		break;
	case STATE_WAIT_COMPLETE:
		AxisStatus = smGetParam(AxisName, "SimpleStatus", &simpleStatus);
		if(SIMPLE_STATUS_IDLE == simpleStatus) {
			State = STATE_IDLE;
		}
		break;
	case STATE_EMERGENCY_RETURN_START:
		//First, lets stop
		AxisStatus = smCommand(AxisName, "ABSTARGET", Position);
		//Set IO //Handled in STATE_FEED
		//setOutput(OUTPUT_CYCLE_START, 0);
		//setOutput(OUTPUT_FEED_FORWARD, 0);
		State = STATE_FEED;
		break;
	case STATE_OVERFLOW:
		State = STATE_EMERGENCY_RETURN_START;
		break;
	case STATE_ERROR:
		setOutput(OUTPUT_FEED_FORWARD, 0);
		setOutput(OUTPUT_CYCLE_START, 0);
		setOutput(OUTPUT_RAPID_RETRACT, 0);
		if(!ErrorText) {
			State = STATE_IDLE;
		}
		break;
	}


	//Check IO
	if(!readInput(INPUT_OVERFLOW)) {
		//Rapid retract
		ErrorText = "OIL TANK FULL";
		State = STATE_EMERGENCY_RETURN_START;
	}
	if(!readInput(INPUT_RAPID_RETRACT)) {
		switch(State) {
		case STATE_IDLE:
			//This is fine, ignore
			break;
		case STATE_ERROR:
			//Reset Errors if possible
			ErrorText = NULL;
			break;
		case STATE_STARTUP:
			//Enable Running
			State = STATE_IDLE;
			break;
		default:
			//Reset while running, Rapid retract
			ErrorText = "EMERGENCY RETRACT WHILE RUNNING";
			State = STATE_EMERGENCY_RETURN_START;
		}
	}

	//TODO Check drive error bits
}

void init() {
	Position = POSITION_DEFAULT;
	Target = TARGET_DEFAULT;
	Feedrate = FEEDRATE_DEFAULT;
	FeedrateOverride = FEEDRATE_OVERRIDE_DEFAULT;
	Spindle = SPINDLE_DEFAULT;
	SpindleOverride = SPINDLE_OVERRIDE_DEFAULT;
}

void initDrive() {
	smint32 statusBits;

	//Test Comminications
	do {
		setOutput(OUTPUT_DRIVE_ON, 0);
		sleep(1);
		setOutput(OUTPUT_DRIVE_ON, 1);
		sleep(4);
		smCloseDevices();
		AxisStatus = smCommand(AxisName, "TESTCOMMUNICATION", 0);
	}while(AxisStatus != SM_OK);


	do {
		AxisStatus = smGetParam(AxisName, "StatusBits", &statusBits);
		AxisStatus = smCommand(AxisName, "CLEARFAULTS", 0);
	}while(!(statusBits & STAT_SERVO_READY));

//	do{
////		setDriveOnOff(0);
//		setOutput(OUTPUT_DRIVE_ON, 0);
//		sleep(1);
////		setDriveOnOff(1);
//		setOutput(OUTPUT_DRIVE_ON, 1);
//		sleep(10);
//		AxisStatus = smCommand(AxisName, "TESTCOMMUNICATION", 0);
//		AxisStatus = smCommand(AxisName, "CLEARFAULTS", 0);
//
//		if(AxisStatus != SM_OK) {
//			continue;
//		}
//
//		AxisStatus = smGetParam(AxisName, "StatusBits", &statusBits);
//		AxisStatus = smCommand(AxisName, "CLEARFAULTS", 0);
//
//		if(!(statusBits & STAT_SERVO_READY)) {
//			continue;
//		}
//
//		break;
//
//	}while(1);


	//Set ReturnData to position
	smSetParam(AxisName, "ReturnDataPayloadType", CAPTURE_ACTUAL_POSITION);

}

int main(int argc, char** argv) {


	//printf("GunDrill Control\n");

	//SigInt Handler
	signal(SIGINT, sigIntHandler);

	//Gtk Init
	gtk_init(&argc, &argv);

	createDisplay();

	init();

	openIO();
	
	initDrive();

	//Numeric Entry Key Press Events
	g_signal_connect(G_OBJECT(numberEntry), "activate", (GCallback)numericInputKeyPressEvent, NULL);
	gtk_widget_set_events(GTK_WIDGET(numberEntry), GDK_KEY_PRESS_MASK);

	g_signal_connect(G_OBJECT(jogDialog), "key_press_event", (GCallback)jogKey_press_event, NULL);
	g_signal_connect(G_OBJECT(jogDialog), "key_release_event", (GCallback)jogKey_release_event, NULL);
	gtk_widget_set_events(GTK_WIDGET(jogDialog), GDK_KEY_PRESS_MASK|GDK_KEY_RELEASE_MASK);

	//Timer Test
	g_timeout_add(TIME_UPDATE_DISPLAY, (GSourceFunc) updateDisplayEvent, NULL);
	g_timeout_add(TIME_UPDATE_POSITIONS, (GSourceFunc) updatePositionEvent, NULL);
	g_timeout_add(TIME_UPDATE_STATE, (GSourceFunc) updateStateEvent, NULL);

	
	//Main Loop
	gtk_main();

	//printf("End\n");
	return 0;
}
