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

int Position;
int TargetPosition;

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
		status = smCommand(axis, "ABSTARGET", 40960);
	}
	if(GDK_KEY_e == event->keyval) {
		status = smCommand(axis, "ABSTARGET", -40960);
	}
	if(GDK_KEY_n == event->keyval) {
		requestNumber();
	}
}

void initDrive() {
	status=smCommand(axis, "TESTCOMMUNICATION", 0);
	if(!SM_OK == status) {
		//Problems
		fprintf(stderr, "Drive not online, status: '%s'\n", statusString(status));
	}

	smSetParam(axis, "ReturnDataPayloadType", 6); //TODO 6 Should be a constant
	smSetParam(axis, "VelocityLimit", 1366);

	status = smCommand(axis, "CLEARFAULTS", 0);
}

void updatePosition() {
	smint32 returnValue;
	smint16 diff;

	//TODO Status update here?
	status = smGetParam(axis, "ReturnDataPayload", &returnValue);
	diff = returnValue-previousDrivePosition;
	Position += diff;
	previousDrivePosition = returnValue;
}

void updateDisplay() {
	gtk_label_set_markup(GTK_LABEL(positionDisplay), g_markup_printf_escaped(positionDisplayMarkup, Position/PositionScale));
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
