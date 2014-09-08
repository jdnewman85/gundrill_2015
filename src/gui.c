//GUI Routines

#include <gtk/gtk.h>
#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/vsd_cmd.h"
#include "gui.h"

//Globals
GtkWidget* mainWindow;
GdkRGBA mainBgColor;
GdkCursor* cursor;
GtkWidget* mainFrame;
GtkWidget* mainFrameLabel;
GtkWidget* mainGrid;
GtkWidget* positionFrame;
GtkWidget* positionFrameLabel;
GtkWidget* positionDisplay;
GtkWidget* targetFrame;
GtkWidget* targetFrameLabel;
GtkWidget* targetDisplay;
GtkWidget* toGoFrame;
GtkWidget* toGoFrameLabel;
GtkWidget* toGoDisplay;
GtkWidget* feedrateFrame;
GtkWidget* feedrateFrameLabel;
GtkWidget* feedrateBox;
GtkWidget* feedrateDisplay;
GtkWidget* feedrateOverride;
GtkWidget* spindleFrame;
GtkWidget* spindleFrameLabel;
GtkWidget* spindleBox;
GtkWidget* spindleDisplay;
GtkWidget* spindleOverride;
GtkWidget* statusFrame;
GtkWidget* statusFrameLabel;
GtkWidget* statusDisplay;
GdkRGBA numberDialogBgColor;
GtkWidget* numberDialog;
GtkWidget* numberEntry;
GtkWidget* numberEntryLabel;

gchar* mainFrameLabelMarkup	= "<span font='15' color='#ffffff'>Testers - Gun Drill</span>";
gchar* positionFrameLabelMarkup	= "<span font='15' color='#ffffff'>Position</span>";
gchar* positionDisplayMarkup	= "<span weight='bold' font='80' color='#ffffff'>%2.4f</span>";
gchar* targetFrameLabelMarkup	= "<span font='15' color='#ffffff'>[1] Target</span>";
gchar* targetDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%2.4f</span>";
gchar* toGoFrameLabelMarkup	= "<span font='15' color='#ffffff'>ToGo</span>";
gchar* toGoDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%2.4f</span>";
gchar* feedrateFrameLabelMarkup	= "<span font='15' color='#ffffff'>[2] Feedrate - IPM</span>";
gchar* feedrateDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%1.1f</span>";
gchar* feedrateOverrideMarkup	= "<span weight='bold' font='30' color='#ffffff'>%3d%%</span>";
gchar* spindleFrameLabelMarkup	= "<span font='15' color='#ffffff'>[3] Spindle - RPM</span>";
gchar* spindleDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%4d</span>";
gchar* spindleOverrideMarkup	= "<span weight='bold' font='30' color='#ffffff'>%3d%%</span>";
gchar* statusFrameLabelMarkup	= "<span font='15' color='#ffffff'>Status</span>";
gchar* statusDisplayMarkup	= "<span weight='bold' font='15' color='#ffffff'>Drive Online</span>";
gchar* numberEntryLabelMarkup	= "<span weight='bold' font='15' color='#000000'>Enter a Value</span>";

void createDisplay() {
	//Main Window
	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_maximize(GTK_WINDOW(mainWindow));

	//Key Press Events
	g_signal_connect(G_OBJECT(mainWindow), "key_press_event", (GCallback)key_press_event, NULL);
	gtk_widget_set_events(GTK_WIDGET(mainWindow), GDK_KEY_PRESS_MASK);

	//Background Color
	mainBgColor.red = 0.0;
	mainBgColor.green = 0.0;
	mainBgColor.blue = 0.0;
	mainBgColor.alpha = 1.0;
	gtk_widget_override_background_color(mainWindow, GTK_STATE_NORMAL, &mainBgColor);

	//Main Frame
	mainFrame = gtk_frame_new(NULL);
	gtk_frame_set_label_align(GTK_FRAME(mainFrame), 1.0, 0.5);
	gtk_container_add(GTK_CONTAINER(mainWindow), mainFrame);

	//Main Frame Label
	mainFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(mainFrameLabel), mainFrameLabelMarkup);
	gtk_frame_set_label_widget(GTK_FRAME(mainFrame), mainFrameLabel);

	//Grid
	mainGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(mainFrame), mainGrid);
	gtk_widget_set_hexpand(mainGrid, TRUE);
	gtk_widget_set_vexpand(mainGrid, TRUE);

	//Position Frame
	positionFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), positionFrame, 0, 0, 9, 1);
	gtk_widget_set_hexpand(positionFrame, TRUE);
	//Position Frame Label
	positionFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(positionFrameLabel), positionFrameLabelMarkup);
	gtk_frame_set_label_widget(GTK_FRAME(positionFrame), positionFrameLabel);
	//Position Display
	positionDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(positionDisplay), g_markup_printf_escaped(positionDisplayMarkup, 99.9999));
	gtk_container_add(GTK_CONTAINER(positionFrame), positionDisplay);


	//Target Frame
	targetFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), targetFrame, 1, 1, 3, 1);
	gtk_widget_set_hexpand(targetFrame, TRUE);
	//Target Frame Label
	targetFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(targetFrameLabel), targetFrameLabelMarkup);
	gtk_frame_set_label_widget(GTK_FRAME(targetFrame), targetFrameLabel);
	//Target Display
	targetDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(targetDisplay), g_markup_printf_escaped(targetDisplayMarkup, 99.9999));
	gtk_container_add(GTK_CONTAINER(targetFrame), targetDisplay);

	//ToGo Frame
	toGoFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), toGoFrame, 5, 1, 3, 1);
	gtk_widget_set_hexpand(toGoFrame, TRUE);
	//ToGo Frame Label
	toGoFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(toGoFrameLabel), toGoFrameLabelMarkup);
	gtk_frame_set_label_widget(GTK_FRAME(toGoFrame), toGoFrameLabel);
	//ToGo Display
	toGoDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(toGoDisplay), g_markup_printf_escaped(toGoDisplayMarkup, 99.9999));
	gtk_container_add(GTK_CONTAINER(toGoFrame), toGoDisplay);

	//Feedrate Frame
	feedrateFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), feedrateFrame, 1, 2, 3, 1);
	gtk_widget_set_hexpand(feedrateFrame, TRUE);
	//Feedrate Frame Label
	feedrateFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(feedrateFrameLabel), feedrateFrameLabelMarkup);
	gtk_frame_set_label_widget(GTK_FRAME(feedrateFrame), feedrateFrameLabel);
	//Feedrate Box
	feedrateBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(feedrateFrame), feedrateBox);
	//Feedrate Display
	feedrateDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(feedrateDisplay), g_markup_printf_escaped(feedrateDisplayMarkup, 9.9));
	gtk_box_pack_start(GTK_BOX(feedrateBox), feedrateDisplay, TRUE, TRUE, 0);
	//Feedrate Override
	feedrateOverride = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(feedrateOverride), g_markup_printf_escaped(feedrateOverrideMarkup, 100));
	gtk_box_pack_start(GTK_BOX(feedrateBox), feedrateOverride, TRUE, TRUE, 0);

	//Spindle Frame
	spindleFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), spindleFrame, 5, 2, 3, 1);
	gtk_widget_set_hexpand(spindleFrame, TRUE);
	//Spindle Frame Label
	spindleFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(spindleFrameLabel), spindleFrameLabelMarkup);
	gtk_frame_set_label_widget(GTK_FRAME(spindleFrame), spindleFrameLabel);
	//Spindle Box
	spindleBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(spindleFrame), spindleBox);
	//Spindle Display
	spindleDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(spindleDisplay), g_markup_printf_escaped(spindleDisplayMarkup, 999));
	gtk_box_pack_start(GTK_BOX(spindleBox), spindleDisplay, TRUE, TRUE, 0);
	//Spindle Override
	spindleOverride = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(spindleOverride), g_markup_printf_escaped(spindleOverrideMarkup, 100));
	gtk_box_pack_start(GTK_BOX(spindleBox), spindleOverride, TRUE, TRUE, 0);

	//Status Frame
	statusFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), statusFrame, 0, 3, 9, 1);
	gtk_widget_set_hexpand(statusFrame, TRUE);
	gtk_widget_set_valign(statusFrame, GTK_ALIGN_BASELINE);
	//Status Frame Label
	statusFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(statusFrameLabel), statusFrameLabelMarkup);
	gtk_frame_set_label_widget(GTK_FRAME(statusFrame), statusFrameLabel);
	//Status Display
	statusDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(statusDisplay), statusDisplayMarkup);
	gtk_container_add(GTK_CONTAINER(statusFrame), statusDisplay);
	gtk_widget_set_halign(statusDisplay, GTK_ALIGN_START);
	
	//Number Entry Window
	numberDialog = gtk_dialog_new();
	numberDialogBgColor.red = 1.0;
	numberDialogBgColor.green = 1.0;
	numberDialogBgColor.blue = 1.0;
	numberDialogBgColor.alpha = 1.0;
	gtk_widget_override_background_color(numberDialog, GTK_STATE_NORMAL, &numberDialogBgColor);
	//Number Entry Label
	numberEntryLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(numberEntryLabel), numberEntryLabelMarkup);
	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(numberDialog))), numberEntryLabel);
	gtk_window_set_position(GTK_WINDOW(numberDialog), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_decorated(GTK_WINDOW(numberDialog), FALSE);
	//Number Entry Widget
	numberEntry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(numberDialog))), numberEntry);

	gtk_widget_show_all(mainWindow);

	//Hide Cursor (Must be after window is shown
	cursor = gdk_cursor_new(GDK_BLANK_CURSOR);
	gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(mainWindow)), cursor);
	g_object_unref(cursor);

}

void requestNumber() {
	gtk_widget_show_all(numberDialog);
	gint result = gtk_dialog_run(GTK_DIALOG(numberDialog));
	gtk_widget_hide(numberDialog);
}
