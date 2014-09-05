#ifndef GUI_H
#define GUI_H

//Globals
GtkWidget* mainWindow;

//Externs
extern char* axis;
extern SM_STATUS status;


static gint key_press_event(GtkWidget *widget, GdkEventKey *event);
void createDisplay();

#endif //GUI_H
