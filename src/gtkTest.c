#include <gtk/gtk.h>

int main(int argc, char ** argv) {
	GtkWidget *window;

	//Init
	gtk_init(&argc, &argv);

	//Main Window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_maximize(GTK_WINDOW(window));
	gtk_widget_show(window);

	//Background Color
	GdkColor color;
	color.red = 0;
	color.green = 0;
	color.blue = 0;
	gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);

	//Hide Cursor
	GdkCursor *cur;
	cur = gdk_cursor_new(GDK_BLANK_CURSOR);
	gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(window)), cur);
	gdk_cursor_unref(cur);

	//Test Label
	GtkWidget *label;
	label = gtk_label_new("<span font='42'>10.34</span>");
	gtk_label_set_markup(GTK_LABEL(label), "<span weight='bold' font='80' color='#ffffff'>10.34</span>");
	gtk_container_add(GTK_CONTAINER(window), label);

	gtk_widget_show_all(window);

	//Main Loop
	gtk_main();

	return 0;
}
