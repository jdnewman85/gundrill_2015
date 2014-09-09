#ifndef GUI_H
#define GUI_H

//Externs
extern GtkWidget* mainWindow;
extern GdkRGBA mainBgColor;
extern GdkCursor* cursor;
extern GtkWidget* mainFrame;
extern GtkWidget* mainFrameLabel;
extern GtkWidget* mainGrid;
extern GtkWidget* positionFrame;
extern GtkWidget* positionFrameLabel;
extern GtkWidget* positionDisplay;
extern GtkWidget* targetFrame;
extern GtkWidget* targetFrameLabel;
extern GtkWidget* targetDisplay;
extern GtkWidget* toTargetFrame;
extern GtkWidget* toTargetFrameLabel;
extern GtkWidget* toTargetDisplay;
extern GtkWidget* feedrateFrame;
extern GtkWidget* feedrateFrameLabel;
extern GtkWidget* feedrateBox;
extern GtkWidget* feedrateDisplay;
extern GtkWidget* feedrateOverrideDisplay;
extern GtkWidget* spindleFrame;
extern GtkWidget* spindleFrameLabel;
extern GtkWidget* spindleBox;
extern GtkWidget* spindleDisplay;
extern GtkWidget* spindleOverrideDisplay;
extern GtkWidget* statusFrame;
extern GtkWidget* statusFrameLabel;
extern GtkWidget* statusDisplay;
extern GdkRGBA dialogBgColor;
extern GtkWidget* numberDialog;
extern GtkWidget* numberEntry;
extern GtkWidget* numberEntryLabel;
extern GtkWidget* jogDialog;
extern GtkWidget* jogDialogFrame;
extern GtkWidget* jogDialogFrameLabel;
extern GtkWidget* jogDialogBox;
extern GtkWidget* jogLabel[6];

extern gchar* positionDisplayMarkup;
extern gchar* targetDisplayMarkup;
extern gchar* toTargetDisplayMarkup;
extern gchar* feedrateDisplayMarkup;
extern gchar* feedrateOverrideDisplayMarkup;
extern gchar* spindleDisplayMarkup;
extern gchar* spindleOverrideDisplayMarkup;
extern gchar* statusDisplayMarkup;
extern gchar* numberEntryLabelMarkup;
extern gchar* jogLabelMarkup[6];

extern gint key_release_event(GtkWidget *widget, GdkEventKey *event);
gboolean updateTimerEvent(gpointer userData);
void createDisplay();
void requestNumber();
void showJogDialog();

#endif //GUI_H
