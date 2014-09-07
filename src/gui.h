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
extern GtkWidget* toGoFrame;
extern GtkWidget* toGoFrameLabel;
extern GtkWidget* toGoDisplay;
extern GtkWidget* feedrateFrame;
extern GtkWidget* feedrateFrameLabel;
extern GtkWidget* feedrateBox;
extern GtkWidget* feedrateDisplay;
extern GtkWidget* feedrateOverride;
extern GtkWidget* spindleFrame;
extern GtkWidget* spindleFrameLabel;
extern GtkWidget* spindleBox;
extern GtkWidget* spindleDisplay;
extern GtkWidget* spindleOverride;
extern GtkWidget* statusFrame;
extern GtkWidget* statusFrameLabel;
extern GtkWidget* statusDisplay;
extern GdkRGBA numberDialogBgColor;
extern GtkWidget* numberDialog;
extern GtkWidget* numberEntry;
extern GtkWidget* numberEntryLabel;

extern char* axis;
extern SM_STATUS status;


extern gint key_press_event(GtkWidget *widget, GdkEventKey *event);
void createDisplay();
void requestNumber();

#endif //GUI_H
