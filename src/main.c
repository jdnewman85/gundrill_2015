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

int State;

int JogFeedrate[] = {JOG_FEEDRATE_1X, JOG_FEEDRATE_10X, JOG_FEEDRATE_100X};

int JogMode = JOG_MODE_DEFAULT;
int JogDirection = JOG_STOP;

void doState() {
	smint32 simpleStatus;

	switch(State) {
	case STATE_IDLE:
		//Check for START? TODO?
		break;
	case STATE_START:
		//Start part, and set into feed
		smSetParam(AxisName, "VelocityLimit", (int)withOverride(Feedrate, FeedrateOverride, FEEDRATE_MIN, FEEDRATE_MAX));
		//Set IO
		setOutput(OUTPUT_CYCLE_START, 1);
		//Reset 0 to current location
		smSetParam(AxisName, "ControlMode", CONTROL_MODE_VELOCITY);
		smSetParam(AxisName, "ControlMode", CONTROL_MODE_POSITION);
		//Set IO
		setOutput(OUTPUT_FEED_FORWARD, 1);
		//Move
		AxisStatus = smCommand(AxisName, "ABSTARGET", Target);
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
			State = STATE_IDLE;
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
		//First, lets stop
		AxisStatus = smCommand(AxisName, "ABSTARGET", Position);
		//Set IO //Handled in STATE_FEED
		//setOutput(OUTPUT_CYCLE_START, 0);
		//setOutput(OUTPUT_FEED_FORWARD, 0);
		State = STATE_FEED;
		break;
	}
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
		smCloseDevices();
		sleep(1);
		AxisStatus = smCommand(AxisName, "TESTCOMMUNICATION", 0);
		sleep(1);
		AxisStatus = smCommand(AxisName, "CLEARFAULTS", 0);
	}while(AxisStatus != SM_OK);


	do {
		AxisStatus = smGetParam(AxisName, "StatusBits", &statusBits);
		AxisStatus = smCommand(AxisName, "CLEARFAULTS", 0);
	}while(!(statusBits & STAT_SERVO_READY));

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
	setDriveOnOff(1);
	
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
