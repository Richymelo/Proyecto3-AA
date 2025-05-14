/*
                Proyecto 3: Suma de Subconjuntos
                Hecho por: Carmen Hidalgo Paz, Jorge Guevara Chavarría y Ricardo Castro Jiménez
                Fecha: Jueves 15 de mayo del 2025

                Esta sección contiene el main, donde se indica lo que tiene que hacer
                cada objeto mostrado en la interfaz. Además, hay una función que muestra los
                rayos y el círculo editado con los valores que ingresa el usuario.
                Después se ordenan los rayos con el algoritmo de ordenamiento escogido
                por el usuario.
*/
#include <gtk/gtk.h>
#include <cairo.h>
#include <stdbool.h>

// Para que no se mueva la línea del panel
void fijar_panel(GtkPaned *panel, GParamSpec *pspec, gpointer user_data) {
    const int pos_fijada = 1000;    // Posición donde se fija la división
    int current_pos = gtk_paned_get_position(panel);
    if (current_pos != pos_fijada) {
        gtk_paned_set_position(panel, pos_fijada);
    }
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;         // Utilizado para obtener los objetos de glade
    GtkWidget *ventana;          // La ventana
    //GtkWidget *area_circulo1;    // El área donde se dibuja el círculo 1
    GtkWidget *panel;            // El panel que divide el área de dibujo y el área de interacción
    //GtkWidget *boton_salida;     // Botón para terminar el programa

    gtk_init(&argc, &argv);

    // Cargar la interfaz de Glade
    builder = gtk_builder_new_from_file("interfaz.glade");

    // La ventana
    ventana = GTK_WIDGET(gtk_builder_get_object(builder, "ventana"));
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    // El panel divisor
    panel = GTK_WIDGET(gtk_builder_get_object(builder, "division"));
    g_signal_connect(panel, "notify::position", G_CALLBACK(fijar_panel), NULL);

    // Mostrar ventana
    gtk_widget_show_all(ventana);
    // Que la ventana utilize toda la pantalla
    gtk_window_fullscreen(GTK_WINDOW(ventana));

    // Correr GTK
    gtk_main();

    g_object_unref(builder);

    return 0;
}