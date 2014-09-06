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

void sigIntHandler() {
	gtk_main_quit();
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

int main(int argc, char** argv) {


	printf("GunDrill Control\n");

	//SigInt Handler
	signal(SIGINT, sigIntHandler);

	//Gtk Init
	gtk_init(&argc, &argv);


	//OLD NEEDS PLACE
	//Drive exists?
	status=smCommand(axis, "TESTCOMMUNICATION", 0);
	if(!SM_OK == status) {
		//Problems
		fprintf(stderr, "Drive not online, status: '%s'\n", statusString(status));
	}

	smSetParam(axis, "ReturnDataPayloadType", 6); //TODO 6 Should be a constant
	smSetParam(axis, "VelocityLimit", 1366);
//	smSetParam(axis, "VelocityLimit", 100);

	status = smCommand(axis, "CLEARFAULTS", 0);
	

	createDisplay();
	


	//Main Loop
	gtk_main();


	smint16 diff = 0;
	smint32 unwrapped = 0;
	smint16 previous = 0;

//	for(;1;) {
//		status = smGetParam(axis, "ReturnDataPayload", &returnValue);
//		diff = returnValue-previous;
//		unwrapped += diff;
//		previous = returnValue;
//		printf("RETURN PAY LOAD: %d\n", unwrapped);
//	}

	printf("End\n");
	return 0;
}
