/*
                Proyecto 3: Suma de Subconjuntos
                Hecho por: Carmen Hidalgo Paz, Jorge Guevara Chavarría y Ricardo Castro Jiménez
                Fecha: Jueves 15 de mayo del 2025

                Esta sección contiene el main, donde se indica lo que tiene que hacer
                cada objeto mostrado en la interfaz.
*/
#include <gtk/gtk.h>
#include <cairo.h>
#include <stdbool.h>
#include "Delta.h"
#include <limits.h>

// Para que no se mueva la línea del panel
void fijar_panel(GtkPaned *panel, GParamSpec *pspec, gpointer user_data) {
    const int pos_fijada = 920;    // Posición donde se fija la división
    int current_pos = gtk_paned_get_position(panel);
    if (current_pos != pos_fijada) {
        gtk_paned_set_position(panel, pos_fijada);
    }
}

// Recorre box_ai para encontrar el mínimo de todos los ai
static int get_min_ai(GtkBox *box_ai) {
    GList *kids = gtk_container_get_children(GTK_CONTAINER(box_ai));
    int min_ai = INT_MAX;

    for (GList *iter = kids; iter; iter = iter->next) {
        GtkWidget *hbox = GTK_WIDGET(iter->data);
        // En cada fila (hbox) busca el SpinButton
        GList *hc = gtk_container_get_children(GTK_CONTAINER(hbox));
        for (GList *hiter = hc; hiter; hiter = hiter->next) {
            if (GTK_IS_SPIN_BUTTON(hiter->data)) {
                int val = gtk_spin_button_get_value_as_int(
                              GTK_SPIN_BUTTON(hiter->data));
                if (val < min_ai) min_ai = val;
            }
        }
        g_list_free(hc);
    }
    g_list_free(kids);

    // Si no encontró ninguno, devuelve 1 para que ∆ ≤ 0 sea válido
    return (min_ai == INT_MAX) ? 1 : min_ai;
}

// Callback que muestra/oculta box_delta y ajusta su cota superior
static void on_delta_toggled(GtkToggleButton *toggle, gpointer user_data) {
    DeltaWidgets *dw = user_data;
    gboolean active = gtk_toggle_button_get_active(toggle);

    // 1) Mostrar u ocultar todo el contenedor
    gtk_widget_set_visible(GTK_WIDGET(dw->box_delta), active);
    if (!active) return;

    // 2) Calcular la cota: ∆ < ai para todo i ⇒ ∆ ≤ min(ai)-1
    int min_ai = get_min_ai(dw->box_ai);

    // 3) Actualizar el Adjustment del spin_delta
    GtkAdjustment *adj = gtk_spin_button_get_adjustment(dw->spin_delta);
    gtk_adjustment_set_lower(adj, 0);
    gtk_adjustment_set_upper(adj, min_ai - 1);

    // 4) Si el valor actual excede la cota, ajústalo
    int d = gtk_spin_button_get_value_as_int(dw->spin_delta);
    if (d > min_ai - 1)
        gtk_spin_button_set_value(dw->spin_delta, min_ai - 1);
}

static void on_ai_changed(GtkSpinButton *spin, gpointer user_data) {
    DeltaWidgets *dw = user_data;
    // Si ∆ está activo, recálcula su límite
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dw->rb_delta))) {
        on_delta_toggled(dw->rb_delta, dw);
    }
}

static void on_size_value_changed(GtkSpinButton *spin, gpointer user_data) {
    DeltaWidgets *dw = user_data;
    GtkBox *box_ai = dw->box_ai;
    int n = gtk_spin_button_get_value_as_int(spin);
    if (n < 3)  n = 3;
    if (n > 12) n = 12;

    // Eliminar hijos previos
    GList *kids = gtk_container_get_children(GTK_CONTAINER(box_ai));
    for (GList *it = kids; it; it = it->next)
        gtk_widget_destroy(GTK_WIDGET(it->data));
    g_list_free(kids);

    // Crear n nuevos GtkSpinButton para ai
    for (int i = 0; i < n; i++) {
        GtkAdjustment *adj = gtk_adjustment_new(1, 1, G_MAXINT, 1, 10, 0);
        GtkWidget *spin_ai = gtk_spin_button_new(adj, 1, 0);

        // Empaqueta etiqueta + spin
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
        char buf[16];
        g_snprintf(buf, sizeof(buf), "a%d:", i+1);
        gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(buf), FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), spin_ai, FALSE, FALSE, 0);

        // Conecta cada ai para que al cambiar actualice ∆
        g_signal_connect(spin_ai, "value-changed",
                         G_CALLBACK(on_ai_changed), dw);

        gtk_box_pack_start(box_ai, hbox, FALSE, FALSE, 2);
        gtk_widget_show_all(hbox);
    }

    // Y también, si ∆ ya está mostrado, actualizarlo
    on_ai_changed(NULL, dw);
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *ventana;
    GtkWidget *panel;
    GtkWidget *boton_salida;
    GtkSpinButton *spin_size;
    DeltaWidgets *dw;

    gtk_init(&argc, &argv);

    // Cargar de la interfaz
    builder = gtk_builder_new_from_file("interfaz.glade");

    // Ventana principal
    ventana = GTK_WIDGET(gtk_builder_get_object(builder, "ventana"));
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Panel divisor (si lo usas para algo)
    panel = GTK_WIDGET(gtk_builder_get_object(builder, "division"));
    g_signal_connect(panel, "notify::position", G_CALLBACK(fijar_panel), NULL);

    // El bóton de terminación del programa
    boton_salida = GTK_WIDGET(gtk_builder_get_object(builder, "buttonFinish"));
    g_signal_connect(boton_salida, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    // Reserva e inicializa tu struct con todos los widgets
    dw = g_new0(DeltaWidgets, 1);
    dw->box_ai     = GTK_BOX(
        gtk_builder_get_object(builder, "box_ai"));
    dw->box_delta  = GTK_BOX(
        gtk_builder_get_object(builder, "box_delta"));
    dw->spin_delta = GTK_SPIN_BUTTON(
        gtk_builder_get_object(builder, "spin_delta"));
    dw->rb_delta   = GTK_TOGGLE_BUTTON(
        gtk_builder_get_object(builder, "varianteDelta"));

    // Conecta el radio ∆ (antes de poblar nada)
    g_signal_connect(dw->rb_delta, "toggled", G_CALLBACK(on_delta_toggled), dw);

    // Conecta el spin de tamaño, pasándole todo el struct
    spin_size = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "size"));
    g_signal_connect(spin_size, "value-changed", G_CALLBACK(on_size_value_changed), dw);

    // Estado inicial de la UI: crear los ai
    on_size_value_changed(spin_size, dw);

    // Mostrar la ventana y pasar a fullscreen
    gtk_widget_show_all(ventana);
    gtk_window_fullscreen(GTK_WINDOW(ventana));
    
    // Estado inicial de la UI: ocultar ∆
    on_delta_toggled(dw->rb_delta, dw);

    // Loop principal
    gtk_main();

    g_object_unref(builder);
    g_free(dw);
    return 0;
}