//GunDrill

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
#define FEEDRATE_OVERRIDE_DEFAULT	100
#define FEEDRATE_OVERRIDE_MAX		150
#define FEEDRATE_OVERRIDE_MIN		50
#define SPINDLE_OVERRIDE_DEFAULT	100
#define SPINDLE_OVERRIDE_MAX		150
#define SPINDLE_OVERRIDE_MIN		50
#define STATUS_LENGTH			100

int Position;
int Target;
int ToGo;
int Feedrate;
int FeedrateOverride;
int Spindle;
int SpindleOverride;
char Status[STATUS_LENGTH];

float PositionScale = 40960.0;

smint16 previousDrivePosition;

void sigIntHandler() {
	gtk_main_quit();
}

gint numericInputKeyPressEvent(GtkWidget *widget, gpointer userData) {
	printf("ACTIVATE!\n");
	gtk_dialog_response(GTK_DIALOG(numberDialog), 1);
}

gint key_press_event(GtkWidget *widget, GdkEventKey *event) {
	printf("KEY!: '%s'\n", gdk_keyval_name(event->keyval));
	if(GDK_KEY_Escape == event->keyval) {
		sigIntHandler();
	}
	if(GDK_KEY_space == event->keyval) {
		AxisStatus = smCommand(AxisName, "ABSTARGET", 40960);
	}
	if(GDK_KEY_e == event->keyval) {
		AxisStatus = smCommand(AxisName, "ABSTARGET", -40960);
	}
	if(GDK_KEY_n == event->keyval) {
		requestNumber();
	}
}

void init() {
	Position = POSITION_DEFAULT;

}

void initDrive() {
	//Test Comminications
	AxisStatus=smCommand(AxisName, "TESTCOMMUNICATION", 0);

	//TODO REMOVE Velocity should be handled elsewhere unless this is needed for homing
	smSetParam(AxisName, "VelocityLimit", 1366);

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
	gtk_label_set_markup(GTK_LABEL(positionDisplay), g_markup_printf_escaped(positionDisplayMarkup, Position));
	gtk_label_set_markup(GTK_LABEL(targetDisplay), g_markup_printf_escaped(targetDisplayMarkup, Target));
	gtk_label_set_markup(GTK_LABEL(toGoDisplay), g_markup_printf_escaped(toGoDisplayMarkup, ToGo));
	gtk_label_set_markup(GTK_LABEL(feedrateDisplay), g_markup_printf_escaped(feedrateDisplayMarkup, Feedrate));
	gtk_label_set_markup(GTK_LABEL(feedrateOverrideDisplay), g_markup_printf_escaped(feedrateOverrideDisplayMarkup, FeedrateOverride));
	gtk_label_set_markup(GTK_LABEL(spindleDisplay), g_markup_printf_escaped(spindleDisplayMarkup, Spindle));
	gtk_label_set_markup(GTK_LABEL(spindleOverrideDisplay), g_markup_printf_escaped(spindleOverrideDisplayMarkup, SpindleOverride));

	//Status
	sprintf(Status, "Drive: %s", statusString(AxisStatus));
	gtk_label_set_markup(GTK_LABEL(statusDisplay), g_markup_printf_escaped(statusDisplayMarkup, Status));
}

gboolean updateTimerEvent(gpointer userData) {
	updatePosition();

	updateDisplay();
	return TRUE;
}

int main(int argc, char** argv) {


	printf("GunDrill Control\n");

	//SigInt Handler
	signal(SIGINT, sigIntHandler);

	//Gtk Init
	gtk_init(&argc, &argv);

	createDisplay();

	initDrive();

	//Numeric Entry Key Press Events
	g_signal_connect(G_OBJECT(numberEntry), "activate", (GCallback)numericInputKeyPressEvent, NULL);
	gtk_widget_set_events(GTK_WIDGET(numberEntry), GDK_KEY_PRESS_MASK);

	//Timer Test
	g_timeout_add(10, (GSourceFunc) updateTimerEvent, NULL);

	
	//Main Loop
	gtk_main();

	printf("End\n");
	return 0;
}
