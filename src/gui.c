//GUI Routines

#include <gtk/gtk.h>
#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/vsd_cmd.h"
#include "gui.h"

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
	//Main Window
	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_maximize(GTK_WINDOW(mainWindow));
	gtk_widget_show(mainWindow);

	//Key Press Events
	g_signal_connect(G_OBJECT(mainWindow), "key_press_event", key_press_event, NULL);
	gtk_widget_set_events(GTK_WIDGET(mainWindow), GDK_KEY_PRESS_MASK);

	//Background Color
	GdkColor mainBgColor;
	mainBgColor.red = 0;
	mainBgColor.green = 0;
	mainBgColor.blue = 0;
	gtk_widget_modify_bg(mainWindow, GTK_STATE_NORMAL, &mainBgColor);

	//Hide Cursor
	GdkCursor* cursor;
	cursor = gdk_cursor_new(GDK_BLANK_CURSOR);
	gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(mainWindow)), cursor);
	gdk_cursor_unref(cursor);

	//Main Frame
	GtkWidget* mainFrame;
	mainFrame = gtk_frame_new(NULL);
	gtk_frame_set_label_align(GTK_FRAME(mainFrame), 1.0, 0.5);
	gtk_container_add(GTK_CONTAINER(mainWindow), mainFrame);

	//Main Frame Label
	GtkWidget* mainFrameLabel;
	mainFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(mainFrameLabel), "<span font='15' color='#ffffff'>Testers - Gun Drill</span>");
	gtk_frame_set_label_widget(GTK_FRAME(mainFrame), mainFrameLabel);

	//Grid
	GtkWidget* mainGrid;
	mainGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(mainFrame), mainGrid);
	gtk_widget_set_hexpand(mainGrid, TRUE);
	gtk_widget_set_vexpand(mainGrid, TRUE);

	//Position Frame
	GtkWidget* positionFrame;
	positionFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), positionFrame, 0, 0, 9, 1);
	gtk_widget_set_hexpand(positionFrame, TRUE);
	//Position Frame Label
	GtkWidget* positionFrameLabel;
	positionFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(positionFrameLabel), "<span font='15' color='#ffffff'>Position</span>");
	gtk_frame_set_label_widget(GTK_FRAME(positionFrame), positionFrameLabel);
	//Position Display
	GtkWidget* positionDisplay;
	positionDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(positionDisplay), "<span weight='bold' font='80' color='#ffffff'>12.3456</span>");
	gtk_container_add(GTK_CONTAINER(positionFrame), positionDisplay);


	//Target Frame
	GtkWidget* targetFrame;
	targetFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), targetFrame, 1, 1, 3, 1);
	gtk_widget_set_hexpand(targetFrame, TRUE);
	//Target Frame Label
	GtkWidget* targetFrameLabel;
	targetFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(targetFrameLabel), "<span font='15' color='#ffffff'>Target</span>");
	gtk_frame_set_label_widget(GTK_FRAME(targetFrame), targetFrameLabel);
	//Target Display
	GtkWidget* targetDisplay;
	targetDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(targetDisplay), "<span weight='bold' font='30' color='#ffffff'>12.3456</span>");
	gtk_container_add(GTK_CONTAINER(targetFrame), targetDisplay);

	//ToGo Frame
	GtkWidget* toGoFrame;
	toGoFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), toGoFrame, 5, 1, 3, 1);
	gtk_widget_set_hexpand(toGoFrame, TRUE);
	//ToGo Frame Label
	GtkWidget* toGoFrameLabel;
	toGoFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(toGoFrameLabel), "<span font='15' color='#ffffff'>ToGo</span>");
	gtk_frame_set_label_widget(GTK_FRAME(toGoFrame), toGoFrameLabel);
	//ToGo Display
	GtkWidget* toGoDisplay;
	toGoDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(toGoDisplay), "<span weight='bold' font='30' color='#ffffff'>65.4321</span>");
	gtk_container_add(GTK_CONTAINER(toGoFrame), toGoDisplay);

	//Feedrate Frame
	GtkWidget* feedrateFrame;
	feedrateFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), feedrateFrame, 1, 2, 3, 1);
	gtk_widget_set_hexpand(feedrateFrame, TRUE);
	//Feedrate Frame Label
	GtkWidget* feedrateFrameLabel;
	feedrateFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(feedrateFrameLabel), "<span font='15' color='#ffffff'>Feedrate - IPM</span>");
	gtk_frame_set_label_widget(GTK_FRAME(feedrateFrame), feedrateFrameLabel);
	//Feedrate Box
	GtkWidget* feedrateBox;
	feedrateBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(feedrateFrame), feedrateBox);
	//Feedrate Display
	GtkWidget* feedrateDisplay;
	feedrateDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(feedrateDisplay), "<span weight='bold' font='30' color='#ffffff'>2.3</span>");
	gtk_box_pack_start(GTK_BOX(feedrateBox), feedrateDisplay, TRUE, TRUE, 0);
	//Feedrate Override
	GtkWidget* feedrateOverride;
	feedrateOverride = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(feedrateOverride), "<span weight='bold' font='30' color='#ffffff'>100%</span>");
	gtk_box_pack_start(GTK_BOX(feedrateBox), feedrateOverride, TRUE, TRUE, 0);

	//Spindle Frame
	GtkWidget* spindleFrame;
	spindleFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), spindleFrame, 5, 2, 3, 1);
	gtk_widget_set_hexpand(spindleFrame, TRUE);
	//Spindle Frame Label
	GtkWidget* spindleFrameLabel;
	spindleFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(spindleFrameLabel), "<span font='15' color='#ffffff'>Spindle - RPM</span>");
	gtk_frame_set_label_widget(GTK_FRAME(spindleFrame), spindleFrameLabel);
	//Spindle Box
	GtkWidget* spindleBox;
	spindleBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(spindleFrame), spindleBox);
	//Spindle Display
	GtkWidget* spindleDisplay;
	spindleDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(spindleDisplay), "<span weight='bold' font='30' color='#ffffff'>500</span>");
	gtk_box_pack_start(GTK_BOX(spindleBox), spindleDisplay, TRUE, TRUE, 0);
	//Spindle Override
	GtkWidget* spindleOverride;
	spindleOverride = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(spindleOverride), "<span weight='bold' font='30' color='#ffffff'>100%</span>");
	gtk_box_pack_start(GTK_BOX(spindleBox), spindleOverride, TRUE, TRUE, 0);

	//Status Frame
	GtkWidget* statusFrame;
	statusFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), statusFrame, 0, 3, 9, 1);
	gtk_widget_set_hexpand(statusFrame, TRUE);
	gtk_widget_set_valign(statusFrame, GTK_ALIGN_BASELINE);
	//Status Frame Label
	GtkWidget* statusFrameLabel;
	statusFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(statusFrameLabel), "<span font='15' color='#ffffff'>Status</span>");
	gtk_frame_set_label_widget(GTK_FRAME(statusFrame), statusFrameLabel);
	//Status Display
	GtkWidget* statusDisplay;
	statusDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(statusDisplay), "<span weight='bold' font='15' color='#ffffff'>Drive Online</span>");
	gtk_container_add(GTK_CONTAINER(statusFrame), statusDisplay);
	gtk_widget_set_halign(statusDisplay, GTK_ALIGN_START);

	gtk_widget_show_all(mainWindow);
}

