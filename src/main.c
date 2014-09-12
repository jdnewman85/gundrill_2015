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
char* ErrorText = NULL;

char ErrorString[100];

void doState() {
	smint32 simpleStatus;
	smint32 statusBits;
	smint32 errorBits;

	switch(State) {
	case STATE_STARTUP:
		StatusText = "Startup - Waiting for Drive";

		AxisStatus = smGetParam(AxisName, "StatusBits", &statusBits);
		if(STAT_SERVO_READY & statusBits) {
			//Everything up

			//Set ReturnData to position
			smSetParam(AxisName, "ReturnDataPayloadType", CAPTURE_ACTUAL_POSITION);

			State = STATE_STARTUP2;
		}

		break;
	case STATE_STARTUP2:
		StatusText = "Startup - Reset to Enable";
		break;
	case STATE_IDLE:
		setOutput(OUTPUT_FEED_FORWARD, 0);
		setOutput(OUTPUT_CYCLE_START, 0);
		setOutput(OUTPUT_RAPID_RETRACT, 0);
		StatusText = "Idle";
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
		handleReset();
	}

//#define FLT_INVALIDCMD	BV(0)
//#define FLT_FOLLOWERROR	BV(1)
//#define FLT_OVERCURRENT BV(2)
//#define FLT_COMMUNICATION BV(3)
//#define FLT_ENCODER	BV(4)
//#define FLT_OVERTEMP BV(5)
//#define FLT_UNDERVOLTAGE BV(6)
//#define FLT_OVERVOLTAGE BV(7)
//#define FLT_PROGRAM BV(8)
//#define FLT_HARDWARE BV(9)
//#define FLT_MEM BV(10)
//#define FLT_INIT BV(11)
//#define FLT_MOTION BV(12)
////#define FLT_PHASESEARCH_OVERCURRENT BV(12)
//#define FLT_RANGE BV(13)
////to make it necessary to clear faults to activate again
//#define FLT_PSTAGE_FORCED_OFF BV(14)

	//Drive Error?
	if(STATE_STARTUP != State) {
		AxisStatus = smGetParam(AxisName, "FaultBits", &errorBits);
		if(errorBits || AxisStatus != SM_OK) {
			sprintf(ErrorString, "Drive Error: %d %d", AxisStatus, (int)errorBits);
			ErrorText = ErrorString;
			State = STATE_STARTUP;
		}
	}
}

void handleReset() {
	switch(State) {
	case STATE_STARTUP:
		//Lets reset the drive, incase it didn't come up
		ErrorText = NULL;
		initDrive();
		break;
	case STATE_IDLE:
		//This is fine, ignore
		break;
	case STATE_ERROR:
		//Reset Errors if possible
		ErrorText = NULL;
		break;
	case STATE_STARTUP2:
		//Enable Running
		State = STATE_IDLE;
		break;
	default:
		//Reset while running, Rapid retract
		ErrorText = "EMERGENCY RETRACT WHILE RUNNING";
		State = STATE_EMERGENCY_RETURN_START;
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
	//Test Comminications
	setOutput(OUTPUT_DRIVE_ON, 0);
	sleep(1);
	setOutput(OUTPUT_DRIVE_ON, 1);
	smCloseDevices();

}

int main(int argc, char** argv) {
	//printf("GunDrill Control\n");

	//SigInt Handler
	signal(SIGINT, sigIntHandler);

	//Gtk Init
	gtk_init(&argc, &argv);

	init();
	openIO();
	initDrive();

	createDisplay();

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
