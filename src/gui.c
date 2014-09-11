//GUI Routines

#include <gtk/gtk.h>
#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/vsd_cmd.h"
#include "main.h"
#include "axis.h"
#include "constant.h"
#include "gui.h"
#include "aux.h"


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
GtkWidget* toTargetFrame;
GtkWidget* toTargetFrameLabel;
GtkWidget* toTargetDisplay;
GtkWidget* feedrateFrame;
GtkWidget* feedrateFrameLabel;
GtkWidget* feedrateBox;
GtkWidget* feedrateDisplay;
GtkWidget* feedrateOverrideDisplay;
GtkWidget* spindleFrame;
GtkWidget* spindleFrameLabel;
GtkWidget* spindleBox;
GtkWidget* spindleDisplay;
GtkWidget* spindleOverrideDisplay;
GtkWidget* statusFrame;
GtkWidget* statusFrameLabel;
GtkWidget* statusDisplay;
GdkRGBA dialogBgColor;
GtkWidget* numberDialog;
GtkWidget* numberEntry;
GtkWidget* numberEntryLabel;
GtkWidget* jogDialog;
GtkWidget* jogDialogFrame;
GtkWidget* jogDialogFrameLabel;
GtkWidget* jogDialogGrid;
GtkWidget* jogLabel[6];

gchar* mainFrameLabelMarkup	= "<span font='15' color='#ffffff'>Testers - Gun Drill</span>";
gchar* positionFrameLabelMarkup	= "<span font='15' color='#ffffff'>Position</span>";
gchar* positionDisplayMarkup	= "<span weight='bold' font='80' color='#ffffff'>%2.4f</span>";
gchar* targetFrameLabelMarkup	= "<span font='15' color='#ffffff'>[1] Target</span>";
gchar* targetDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%2.4f</span>";
gchar* toTargetFrameLabelMarkup	= "<span font='15' color='#ffffff'>ToTarget</span>";
gchar* toTargetDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%2.4f</span>";
gchar* feedrateFrameLabelMarkup	= "<span font='15' color='#ffffff'>[2] Feedrate IPM [/ *]</span>";
gchar* feedrateDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%1.1f</span>";
gchar* feedrateOverrideDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%3d%%</span>";
gchar* spindleFrameLabelMarkup	= "<span font='15' color='#ffffff'>[3] Spindle RPM [- +]</span>";
gchar* spindleDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%4d</span>";
gchar* spindleOverrideDisplayMarkup	= "<span weight='bold' font='30' color='#ffffff'>%3d%%</span>";
gchar* statusFrameLabelMarkup	= "<span font='15' color='#ffffff'>Status - [R]eset</span>";
gchar* statusDisplayMarkup	= "<span weight='bold' font='15' color='#ffffff'>%s</span>";
gchar* numberEntryLabelMarkup	= "<span weight='bold' font='15' color='#000000'>Enter a %s</span>";
gchar* jogDialogFrameLabelMarkup= "<span weight='bold' font='40' color='#000000'>JOG</span>";
gchar* jogLabelMarkup[6]	= {"<span weight='bold' font='40' color='%s'>   1X </span>", 
				   "<span weight='bold' font='40' color='%s'>  10X </span>",
				   "<span weight='bold' font='40' color='%s'> 100X </span>",
				   "<span weight='bold' font='40' color='%s'> .001 </span>",
				   "<span weight='bold' font='40' color='%s'>  .01 </span>",
				   "<span weight='bold' font='40' color='%s'>   .1 </span>"};

void createDisplay() {
	//Main Window
	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_maximize(GTK_WINDOW(mainWindow));

	//Key Press Events
	g_signal_connect(G_OBJECT(mainWindow), "key_release_event", (GCallback)key_release_event, NULL);
	gtk_widget_set_events(GTK_WIDGET(mainWindow), GDK_KEY_RELEASE_MASK);

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

	//ToTarget Frame
	toTargetFrame = gtk_frame_new(NULL);
	gtk_grid_attach(GTK_GRID(mainGrid), toTargetFrame, 5, 1, 3, 1);
	gtk_widget_set_hexpand(toTargetFrame, TRUE);
	//ToTarget Frame Label
	toTargetFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(toTargetFrameLabel), toTargetFrameLabelMarkup);
	gtk_frame_set_label_widget(GTK_FRAME(toTargetFrame), toTargetFrameLabel);
	//ToTarget Display
	toTargetDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(toTargetDisplay), g_markup_printf_escaped(toTargetDisplayMarkup, 99.9999));
	gtk_container_add(GTK_CONTAINER(toTargetFrame), toTargetDisplay);

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
	feedrateOverrideDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(feedrateOverrideDisplay), g_markup_printf_escaped(feedrateOverrideDisplayMarkup, 100));
	gtk_box_pack_start(GTK_BOX(feedrateBox), feedrateOverrideDisplay, TRUE, TRUE, 0);

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
	spindleOverrideDisplay = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(spindleOverrideDisplay), g_markup_printf_escaped(spindleOverrideDisplayMarkup, 100));
	gtk_box_pack_start(GTK_BOX(spindleBox), spindleOverrideDisplay, TRUE, TRUE, 0);

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
	dialogBgColor.red = 1.0;
	dialogBgColor.green = 1.0;
	dialogBgColor.blue = 1.0;
	dialogBgColor.alpha = 1.0;
	gtk_widget_override_background_color(numberDialog, GTK_STATE_NORMAL, &dialogBgColor);
	//Number Entry Label
	numberEntryLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(numberEntryLabel), numberEntryLabelMarkup);
	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(numberDialog))), numberEntryLabel);
	gtk_window_set_position(GTK_WINDOW(numberDialog), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_decorated(GTK_WINDOW(numberDialog), FALSE);
	//Number Entry Widget
	numberEntry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(numberDialog))), numberEntry);

	//Jog Window
	jogDialog = gtk_dialog_new();
	dialogBgColor.red = 1.0;
	dialogBgColor.green = 1.0;
	dialogBgColor.blue = 1.0;
	dialogBgColor.alpha = 1.0;
	gtk_widget_override_background_color(jogDialog, GTK_STATE_NORMAL, &dialogBgColor);
	//Jog Frame
	jogDialogFrame = gtk_frame_new(NULL);
	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(jogDialog))), jogDialogFrame);
	//Jog Frame Label
	jogDialogFrameLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(jogDialogFrameLabel), jogDialogFrameLabelMarkup);
	gtk_frame_set_label_widget(GTK_FRAME(jogDialogFrame), jogDialogFrameLabel);
	//Jog Grid
	jogDialogGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(jogDialogFrame), jogDialogGrid);
	//Jog Label
	jogLabel[0] = gtk_label_new(NULL);
	jogLabel[1] = gtk_label_new(NULL);
	jogLabel[2] = gtk_label_new(NULL);
	jogLabel[3] = gtk_label_new(NULL);
	jogLabel[4] = gtk_label_new(NULL);
	jogLabel[5] = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(jogLabel[0]), g_markup_printf_escaped(jogLabelMarkup[0], "#AAAAAA"));
	gtk_label_set_markup(GTK_LABEL(jogLabel[1]), g_markup_printf_escaped(jogLabelMarkup[1], "#AAAAAA"));
	gtk_label_set_markup(GTK_LABEL(jogLabel[2]), g_markup_printf_escaped(jogLabelMarkup[2], "#AAAAAA"));
	gtk_label_set_markup(GTK_LABEL(jogLabel[3]), g_markup_printf_escaped(jogLabelMarkup[3], "#AAAAAA"));
	gtk_label_set_markup(GTK_LABEL(jogLabel[4]), g_markup_printf_escaped(jogLabelMarkup[4], "#AAAAAA"));
	gtk_label_set_markup(GTK_LABEL(jogLabel[5]), g_markup_printf_escaped(jogLabelMarkup[5], "#AAAAAA"));
	gtk_grid_attach(GTK_GRID(jogDialogGrid), jogLabel[0], 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(jogDialogGrid), jogLabel[1], 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(jogDialogGrid), jogLabel[2], 2, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(jogDialogGrid), jogLabel[3], 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(jogDialogGrid), jogLabel[4], 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(jogDialogGrid), jogLabel[5], 2, 1, 1, 1);
	gtk_window_set_position(GTK_WINDOW(jogDialog), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_decorated(GTK_WINDOW(jogDialog), FALSE);

	//Show main window
	gtk_widget_show_all(mainWindow);

	//Hide Cursor (Must be after window is shown
	cursor = gdk_cursor_new(GDK_BLANK_CURSOR);
	gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(mainWindow)), cursor);
	g_object_unref(cursor);
}

void requestNumber() {
	gtk_widget_show_all(numberDialog);
	gtk_entry_set_text(GTK_ENTRY(numberEntry), "");
	gtk_dialog_run(GTK_DIALOG(numberDialog));
	gtk_widget_hide(numberDialog);
}

void showJogDialog() {
	gtk_widget_show_all(jogDialog);
	gtk_dialog_run(GTK_DIALOG(jogDialog));
	gtk_widget_hide(jogDialog);
}

void updateDisplay() {
	//NOTE: CHECK DISPLAY PRINTF TYPES!
	gtk_label_set_markup(GTK_LABEL(positionDisplay), g_markup_printf_escaped(positionDisplayMarkup, Position/CntPerInch));
	gtk_label_set_markup(GTK_LABEL(targetDisplay), g_markup_printf_escaped(targetDisplayMarkup, Target/CntPerInch));
	gtk_label_set_markup(GTK_LABEL(toTargetDisplay), g_markup_printf_escaped(toTargetDisplayMarkup, (Target-Position)/CntPerInch));
	gtk_label_set_markup(GTK_LABEL(feedrateDisplay), g_markup_printf_escaped(feedrateDisplayMarkup, withOverride(Feedrate, FeedrateOverride, FEEDRATE_MIN, FEEDRATE_MAX)/VelPerIPM));
	gtk_label_set_markup(GTK_LABEL(feedrateOverrideDisplay), g_markup_printf_escaped(feedrateOverrideDisplayMarkup, FeedrateOverride));
	gtk_label_set_markup(GTK_LABEL(spindleDisplay), g_markup_printf_escaped(spindleDisplayMarkup, (int)withOverride(Spindle, SpindleOverride, SPINDLE_MIN, SPINDLE_MAX)));
	gtk_label_set_markup(GTK_LABEL(spindleOverrideDisplay), g_markup_printf_escaped(spindleOverrideDisplayMarkup, SpindleOverride));

	//Status
	sprintf(Status, "Drive: %s", statusString(AxisStatus));
	gtk_label_set_markup(GTK_LABEL(statusDisplay), g_markup_printf_escaped(statusDisplayMarkup, Status));
}

