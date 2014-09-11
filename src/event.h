#ifndef EVENT_H
#define EVENT_H

gint numericInputKeyPressEvent(GtkWidget *widget, gpointer userData);
gint jogKey_press_event(GtkWidget *widget, GdkEventKey *event);
gint jogKey_release_event(GtkWidget *widget, GdkEventKey *event);
gint key_release_event(GtkWidget *widget, GdkEventKey *event);
gboolean updateDisplayEvent(gpointer userData);
gboolean updatePositionEvent(gpointer userData);
gboolean updateStateEvent(gpointer userData);

#endif //EVENT_H
