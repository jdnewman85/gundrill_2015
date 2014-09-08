//GunDrill

//TODO Should have a function to handle drive status for every function that can return it. If it returns bad, abort
//TODO Reset for drive
//TODO Fix ToGo
//TODO Error better
//TODO Feedhold?
//TODO MIN MAX LIMIT

#include <time.h>
#include <stdio.h>
#include <signal.h>

#include <gtk/gtk.h>

#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/simplemotion_private.h"
#include "SimpleMotion/vsd_cmd.h"
#include "axis.h"
#include "gui.h"

#define POSITION_DEFAULT		0
#define TARGET_DEFAULT			1000000
#define TARGET_MIN			0
#define TARGET_MAX			1000000
#define TOGO_DEFAULT			0
#define FEEDRATE_DEFAULT		136
#define FEEDRATE_MAX			1366
#define FEEDRATE_MIN			0
#define FEEDRATE_OVERRIDE_DEFAULT	100
#define FEEDRATE_OVERRIDE_MAX		120
#define FEEDRATE_OVERRIDE_MIN		10
#define FEEDRATE_OVERRIDE_INC		5
#define SPINDLE_DEFAULT			100
#define SPINDLE_MAX			1000
#define SPINDLE_MIN			100
#define SPINDLE_OVERRIDE_DEFAULT	100
#define SPINDLE_OVERRIDE_MAX		120
#define SPINDLE_OVERRIDE_MIN		50
#define SPINDLE_OVERRIDE_INC		5
#define STATUS_LENGTH			100

#define POSITION_HOME			0
#define FEEDRATE_RAPID			1366

#define STATE_IDLE	0
#define STATE_START	1
#define STATE_FEED	2
#define STATE_RETURN	3

#define SIMPLE_STATUS_MOVING	0
#define SIMPLE_STATUS_ERROR	1
#define SIMPLE_STATUS_IDLE	2

#define TIME_UPDATE_DISPLAY	50
#define TIME_UPDATE_STATE	250

#define INPUT_TYPE_NONE		0
#define INPUT_TYPE_TARGET	1
#define INPUT_TYPE_FEEDRATE	2
#define INPUT_TYPE_SPINDLE	3


int Position;
int Target;
int ToGo;
int Feedrate;
int FeedrateOverride;
int Spindle;
int SpindleOverride;
char Status[STATUS_LENGTH];

float PositionScale = 40960.0;

int State;
int InputType;

smint16 previousDrivePosition;

void sigIntHandler() {
	gtk_main_quit();
}

gint numericInputKeyPressEvent(GtkWidget *widget, gpointer userData) {
	printf("Set Number!!\n");
	gtk_dialog_response(GTK_DIALOG(numberDialog), 1);

	//Actually set our data
	//TODO Scaling
	switch(InputType) {
	case INPUT_TYPE_TARGET:
		Target = atoll(gtk_entry_get_text(GTK_ENTRY(numberEntry)));
		if(Target > TARGET_MAX) {Target = TARGET_MAX;}
		if(Target < TARGET_MIN) {Target = TARGET_MIN;}
		break;
	case INPUT_TYPE_FEEDRATE:
		Feedrate = atoll(gtk_entry_get_text(GTK_ENTRY(numberEntry)));
		if(Feedrate > FEEDRATE_MAX) {Feedrate = FEEDRATE_MAX;}
		if(Feedrate < FEEDRATE_MIN) {Feedrate = FEEDRATE_MIN;}
		break;
	case INPUT_TYPE_SPINDLE:
		Spindle = atoll(gtk_entry_get_text(GTK_ENTRY(numberEntry)));
		if(Spindle > SPINDLE_MAX) {Spindle = SPINDLE_MAX;}
		if(Spindle < SPINDLE_MIN) {Spindle = SPINDLE_MIN;}
		break;
	}
	InputType = INPUT_TYPE_NONE;
}

gint key_release_event(GtkWidget *widget, GdkEventKey *event) {
	printf("KEY!: '%s'\n", gdk_keyval_name(event->keyval));
	switch(event->keyval) {
	case GDK_KEY_Escape:
		sigIntHandler();
		break;
	//Set Target
	case GDK_KEY_1:
		if(STATE_IDLE == State) {
			InputType = INPUT_TYPE_TARGET;
			requestNumber();
		}
		break;
	//Set Feedrate
	case GDK_KEY_2:
		if(STATE_IDLE == State) {
			InputType = INPUT_TYPE_FEEDRATE;
			requestNumber();
		}
		break;
	//Set Spindle
	case GDK_KEY_3:
		if(STATE_IDLE == State) {
			InputType = INPUT_TYPE_SPINDLE;
			requestNumber();
		}
		break;
	case GDK_KEY_space:
		if(STATE_IDLE == State) {
			State = STATE_START;
		}
		break;
	//Feedrate Override
	case GDK_KEY_KP_Divide:
		FeedrateOverride -= FEEDRATE_OVERRIDE_INC;
		if(FeedrateOverride < FEEDRATE_OVERRIDE_MIN) {FeedrateOverride = FEEDRATE_OVERRIDE_MIN;}
		break;
	case GDK_KEY_KP_Multiply:
		FeedrateOverride += FEEDRATE_OVERRIDE_INC;
		if(FeedrateOverride > FEEDRATE_OVERRIDE_MAX) {FeedrateOverride = FEEDRATE_OVERRIDE_MAX;}
		break;
	//Spindle Override
	case GDK_KEY_KP_Subtract:
		SpindleOverride -= SPINDLE_OVERRIDE_INC;
		if(SpindleOverride < SPINDLE_OVERRIDE_MIN) {SpindleOverride = SPINDLE_OVERRIDE_MIN;}
		break;
	case GDK_KEY_KP_Add:
		SpindleOverride += SPINDLE_OVERRIDE_INC;
		if(SpindleOverride > SPINDLE_OVERRIDE_MAX) {SpindleOverride = SPINDLE_OVERRIDE_MAX;}
		break;
	case GDK_KEY_r:
		AxisStatus = smCommand(AxisName, "CLEARFAULTS", 0);
		break;
	}
}

float withOverride(float value, float override, float min, float max) {
	float temp;
	temp = value*(override/100.0);
	if(temp > max) {
		temp = max;
	}
	if(temp < min) {
		temp = min;
	}
	return temp;
}

void doState() {
	smint32 simpleStatus;

	switch(State) {
	case STATE_IDLE:
		//Check for START? TODO?
		break;
	case STATE_START:
		//Start part, and set into feed
		//Set feedrate TODO Proper
		smSetParam(AxisName, "VelocityLimit", (int)withOverride(Feedrate, FeedrateOverride, FEEDRATE_MIN, FEEDRATE_MAX)); //TODO Scale
		//TODO Set IO status
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
			//Good, TODO Dwell at all?
			//Rapid back
			smSetParam(AxisName, "VelocityLimit", FEEDRATE_RAPID);
			AxisStatus = smCommand(AxisName, "ABSTARGET", POSITION_HOME);
			State = STATE_RETURN;
		}
		break;
	case STATE_RETURN:
		//Returning, wait for move complete, then idle
		//Move Complete?
		AxisStatus = smGetParam(AxisName, "SimpleStatus", &simpleStatus);
		if(SIMPLE_STATUS_IDLE == simpleStatus) {
			//Good, time to go idle
			//TODO Parts counter?
			State = STATE_IDLE;
		}
		break;
	}
}

void init() {
	Position = POSITION_DEFAULT;
	Target = TARGET_DEFAULT;
	ToGo = TOGO_DEFAULT;
	Feedrate = FEEDRATE_DEFAULT;
	FeedrateOverride = FEEDRATE_OVERRIDE_DEFAULT;
	Spindle = SPINDLE_DEFAULT;
	SpindleOverride = SPINDLE_OVERRIDE_DEFAULT;
}

void initDrive() {
	//Test Comminications
	AxisStatus=smCommand(AxisName, "TESTCOMMUNICATION", 0);

	//TODO REMOVE Velocity should be handled elsewhere unless this is needed for homing
//	smSetParam(AxisName, "VelocityLimit", 1366);

	//Set ReturnData to position
	smSetParam(AxisName, "ReturnDataPayloadType", 6); //TODO 6 Should be a constant

	//TODO Remove?
	//AxisStatus = smCommand(AxisName, "CLEARFAULTS", 0);
}

void updatePosition() {
	smint32 returnValue;
	smint16 diff;

	//TODO Status update here?
	AxisStatus = smGetParam(AxisName, "ReturnDataPayload", &returnValue);
	diff = returnValue-previousDrivePosition;
	Position += diff;
	previousDrivePosition = returnValue;
}

void updateDisplay() {
	//NOTE: CHECK DISPLAY PRINTF TYPES!
	gtk_label_set_markup(GTK_LABEL(positionDisplay), g_markup_printf_escaped(positionDisplayMarkup, Position/1.0));
	gtk_label_set_markup(GTK_LABEL(targetDisplay), g_markup_printf_escaped(targetDisplayMarkup, Target/1.0));
	gtk_label_set_markup(GTK_LABEL(toTargetDisplay), g_markup_printf_escaped(toTargetDisplayMarkup, (Target-Position)/1.0));
	gtk_label_set_markup(GTK_LABEL(feedrateDisplay), g_markup_printf_escaped(feedrateDisplayMarkup, withOverride(Feedrate, FeedrateOverride, FEEDRATE_MIN, FEEDRATE_MAX)));
	gtk_label_set_markup(GTK_LABEL(feedrateOverrideDisplay), g_markup_printf_escaped(feedrateOverrideDisplayMarkup, FeedrateOverride));
	gtk_label_set_markup(GTK_LABEL(spindleDisplay), g_markup_printf_escaped(spindleDisplayMarkup, (int)withOverride(Spindle, SpindleOverride, SPINDLE_MIN, SPINDLE_MAX)));
	gtk_label_set_markup(GTK_LABEL(spindleOverrideDisplay), g_markup_printf_escaped(spindleOverrideDisplayMarkup, SpindleOverride));

	//Status
	sprintf(Status, "Drive: %s", statusString(AxisStatus));
	gtk_label_set_markup(GTK_LABEL(statusDisplay), g_markup_printf_escaped(statusDisplayMarkup, Status));
}

gboolean updateDisplayEvent(gpointer userData) {
	updatePosition();
	updateDisplay();

	return TRUE;
}

gboolean updateStateEvent(gpointer userData) {
	doState();

	return TRUE;
}

int main(int argc, char** argv) {


	printf("GunDrill Control\n");

	//SigInt Handler
	signal(SIGINT, sigIntHandler);

	//Gtk Init
	gtk_init(&argc, &argv);

	createDisplay();

	init();

	initDrive();

	//Numeric Entry Key Press Events
	g_signal_connect(G_OBJECT(numberEntry), "activate", (GCallback)numericInputKeyPressEvent, NULL);
	gtk_widget_set_events(GTK_WIDGET(numberEntry), GDK_KEY_PRESS_MASK);

	//Timer Test
	g_timeout_add(TIME_UPDATE_DISPLAY, (GSourceFunc) updateDisplayEvent, NULL);
	g_timeout_add(TIME_UPDATE_STATE, (GSourceFunc) updateStateEvent, NULL);

	
	//Main Loop
	gtk_main();

	printf("End\n");
	return 0;
}
