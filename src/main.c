#include <stdio.h>
#include <gtk/gtk.h>

void activated(GtkApplication * app, gpointer user_data) {
    GtkWidget * window;
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "IDET");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    gtk_widget_show(window);
}

int main(int argc, char * argv[]) {
    GtkApplication * idetstartup;
    idetstartup = gtk_application_new("xyz.powpingdone.idet", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(idetstartup, "activate", G_CALLBACK(activated), NULL);
    int status = g_application_run(G_APPLICATION(idetstartup), argc, argv);
    g_object_unref(idetstartup);
    return status;
}
