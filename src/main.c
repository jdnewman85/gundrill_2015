//GunDrill

#include <time.h>
#include <stdio.h>
#include <signal.h>

#include <gtk-3.0/gtk/gtk.h>

#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/simplemotion_private.h"
#include "SimpleMotion/vsd_cmd.h"

//Globals
char* axis = "TTL232R";
SM_STATUS status;
smint32 returnValue;

int waitForMoveDone(char *axis) {
	smint32 status;
	SM_STATUS cmdstat;
	do {
		cmdstat = smGetParam(axis, "SimpleStatus", &status);
		//printf("status=%d, cmdstat=%d\n", status, cmdstat);
	} while(status==0 && cmdstat==SM_OK);

	if(status == 2) {
		return smtrue;
	} else {
		return smfalse;
	}

}

const char* statusString(SM_STATUS status) {
	switch(status) { 
		case SM_OK:
			return "SM_OK";
		case SM_ERR_NODEVICE:
			return "SM_ERR_NODEVICE";
		case SM_ERR_BUS:
			return "SM_ERR_BUS";
		case SM_ERR_COMMUNICATION:
			return "SM_ERR_COMMUNICATION";
		case SM_ERR_PARAMETER:
			return "SM_ERR_PARAMETER";
	}
	
	//default
	return "UNKNOWN SM_STATUS";
}

void sigIntHandler() {
	gtk_main_quit();
}

static gint key_press_event(GtkWidget *widget, GdkEventKey *event) {
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
}

void createDisplay() {
	GtkWidget* window;

	//Main Window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_maximize(GTK_WINDOW(window));
	gtk_widget_show(window);

	//Key Press Events
	g_signal_connect(G_OBJECT(window), "key_press_event", key_press_event, NULL);
	gtk_widget_set_events(GTK_WIDGET(window), GDK_KEY_PRESS_MASK);

	//Background Color
	GdkColor color;
	color.red = 0;
	color.green = 0;
	color.blue = 0;
	gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);

	//Hide Cursor
	GdkCursor* cur;
	cur = gdk_cursor_new(GDK_BLANK_CURSOR);
	gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(window)), cur);
	gdk_cursor_unref(cur);

	//Frame
	GtkWidget* frame;
	frame = gtk_frame_new(NULL);
	gtk_container_add(GTK_CONTAINER(window), frame);

	//Frame Label
	GtkWidget* frameLabel;
	frameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(frameLabel), "<span font='30' color='#ffffff'>Gun Drill</span>");
	gtk_frame_set_label_widget(GTK_FRAME(frame), frameLabel);

	//Test Label
	GtkWidget* label;
	label = gtk_label_new("<span font='42'>10.34</span>");
	gtk_label_set_markup(GTK_LABEL(label), "<span weight='bold' font='80' color='#ffffff'>10.34</span>");
	gtk_container_add(GTK_CONTAINER(frame), label);

	//StatusBar
	GtkWidget* statusBar;
	statusBar = gtk_statusbar_new();
	gtk_container_add(GTK_CONTAINER(frame), statusBar);

	gtk_widget_show_all(window);
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
