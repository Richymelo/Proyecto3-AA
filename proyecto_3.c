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
#include <limits.h>
#include <glib.h>
#include "Widgets.h"
#include "Variantes.h"

int nodos = 0;
int soluciones = 0;

// Para que no se mueva la línea del panel
void fijar_panel(GtkPaned *panel, GParamSpec *pspec, gpointer user_data) {
    const int pos_fijada = 920;    // Posición donde se fija la división
    int current_pos = gtk_paned_get_position(panel);
    if (current_pos != pos_fijada) {
        gtk_paned_set_position(panel, pos_fijada);
    }
}

// Recorre box_ai para encontrar el mínimo de todos los ai
static int get_min_ai(AppWidgets *w) {
    GList *kids = gtk_container_get_children(GTK_CONTAINER(w->box_ai));
    int min_ai = INT_MAX;
    for (GList *iter = kids; iter; iter = iter->next) {
        GtkWidget *hbox = GTK_WIDGET(iter->data);
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
    return (min_ai == INT_MAX) ? 1 : min_ai;
}

// Callback que muestra/oculta box_delta y ajusta su cota superior
static void on_delta_toggled(GtkToggleButton *toggle, gpointer user_data) {
    AppWidgets *w = user_data;
    gboolean active = gtk_toggle_button_get_active(toggle);

    // Mostrar u ocultar el contenedor delta
    gtk_widget_set_visible(GTK_WIDGET(w->box_delta), active);
    if (!active) return;

    // Ajustar rango de spin_delta: ∆ ≤ min(ai)-1
    int min_ai = get_min_ai(w);
    GtkAdjustment *adj = gtk_spin_button_get_adjustment(w->spin_delta);
    gtk_adjustment_set_lower(adj, 0);
    gtk_adjustment_set_upper(adj, min_ai - 1);

    // Si el valor actual excede, corregirlo
    int d = gtk_spin_button_get_value_as_int(w->spin_delta);
    if (d > min_ai - 1)
        gtk_spin_button_set_value(w->spin_delta, min_ai - 1);
}


static void on_ai_changed(GtkSpinButton *spin, gpointer user_data) {
    AppWidgets *w = user_data;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->rb_delta))) {
        on_delta_toggled(w->rb_delta, w);
    }
}

// Callback que, al cambiar spin_i, fuerza que spin_{i+1} ≥ spin_i
static void
on_prev_ai_changed(GtkSpinButton *spin_prev, gpointer user_data)
{
    GtkSpinButton *spin_next = GTK_SPIN_BUTTON(user_data);
    int val_prev = gtk_spin_button_get_value_as_int(spin_prev);

    // Ajuste el límite inferior de spin_next
    GtkAdjustment *adj_next = gtk_spin_button_get_adjustment(spin_next);
    gtk_adjustment_set_lower(adj_next, val_prev);

    // Si el valor actual de spin_next es menor, cámbialo
    int val_next = gtk_spin_button_get_value_as_int(spin_next);
    if (val_next < val_prev)
        gtk_spin_button_set_value(spin_next, val_prev);
}

static void on_size_value_changed(GtkSpinButton *spin, gpointer user_data) {
    AppWidgets *w = user_data;
    int n = gtk_spin_button_get_value_as_int(spin);
    if (n < 3)  n = 3;
    if (n > 12) n = 12;

    // Borra entradas anteriores
    GList *kids = gtk_container_get_children(GTK_CONTAINER(w->box_ai));
    for (GList *it = kids; it; it = it->next)
        gtk_widget_destroy(GTK_WIDGET(it->data));
    g_list_free(kids);

    // VAMOS a guardar cada spin en un array temporal
    GPtrArray *spins = g_ptr_array_new();
    // Crea n nuevos spinbuttons para a_i
    for (int i = 0; i < n; i++) {
        GtkAdjustment *adj = gtk_adjustment_new(1, 1, G_MAXINT, 1, 10, 0);
        GtkWidget *spin_ai = gtk_spin_button_new(adj, 1, 0);

        // Empaqueta etiqueta + spin
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
        char buf[16];
        g_snprintf(buf, sizeof(buf), "a%d:", i+1);
        gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(buf), FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), spin_ai, FALSE, FALSE, 0);

        // Conecta para recálculo de ∆
        g_signal_connect(spin_ai, "value-changed",
                         G_CALLBACK(on_ai_changed), w);
        
        // guarda puntero
        g_ptr_array_add(spins, spin_ai);

        gtk_box_pack_start(GTK_BOX(w->box_ai), hbox, FALSE, FALSE, 2);
        gtk_widget_show_all(hbox);
    }

    // ahora recorre spins[0..n-2] y conecta con spins[i+1]
    for (guint i = 0; i + 1 < spins->len; i++) {
        GtkSpinButton *prev = g_ptr_array_index(spins, i);
        GtkSpinButton *next = g_ptr_array_index(spins, i+1);
        g_signal_connect(prev, "value-changed",
                         G_CALLBACK(on_prev_ai_changed),
                         next);
    }

    g_ptr_array_free(spins, TRUE);

    // Si ∆ ya está activo, actualízalo
    on_ai_changed(NULL, w);
}

static void
on_execute_clicked(GtkButton *btn, gpointer data)
{
    AppWidgets *w = data;
    extern int nodos, soluciones;

    // 1) Leer A desde box_ai
    GList *kids = gtk_container_get_children(
                      GTK_CONTAINER(w->box_ai));
    int n = g_list_length(kids);
    int *A = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        GtkBox *hrow = GTK_BOX(g_list_nth_data(kids, i));
        GList *rowkids = gtk_container_get_children(
                             GTK_CONTAINER(hrow));
        if (g_list_length(rowkids) >= 2) {
            GtkSpinButton *sp = GTK_SPIN_BUTTON(
                g_list_nth_data(rowkids, 1));
            A[i] = gtk_spin_button_get_value_as_int(sp);
        } else {
            A[i] = 0;
        }
        g_list_free(rowkids);
    }
    g_list_free(kids);

    // 2) Leer W y Δ
    int W = gtk_spin_button_get_value_as_int(w->spin_w);
    int delta = gtk_toggle_button_get_active(
                    GTK_TOGGLE_BUTTON(w->rb_delta))
                ? gtk_spin_button_get_value_as_int(w->spin_delta)
                : 0;

    // 3) (Re)crear suffix_sum **global**
    if (suffix_sum) {
        free(suffix_sum);
        suffix_sum = NULL;
    }
    suffix_sum = malloc(sizeof(int) * (n + 1));
    suffix_sum[n] = 0;
    for (int i = n - 1; i >= 0; --i) {
        suffix_sum[i] = A[i] + suffix_sum[i + 1];
    }

    // 4) Reset y preparar sol_list
    nodos = soluciones = 0;
    GPtrArray *sol_list =
        g_ptr_array_new_with_free_func(g_free);
    int actual_idx[12];

    // 5) Ejecutar backtracking
    if (gtk_toggle_button_get_active(
          GTK_TOGGLE_BUTTON(w->rb_v3)))
    {
        sumaSubconjuntosV3_collect(
            A, n, W, 0, actual_idx, 0, 0, sol_list);
    }
    else
    {
        sumaSubconjuntosV4_collect(
            A, n, W, 0, actual_idx, 0, 0, sol_list);
    }

    // 6) Actualizar labels
    gtk_label_set_text(w->lbl_count,
        g_strdup_printf("Soluciones: %u", sol_list->len));
    gtk_label_set_text(w->lbl_nodes,
        g_strdup_printf("Nodos visitados: %d", nodos));

    // 7) Limpiar viejas filas
    GList *old_rows = gtk_container_get_children(
                         GTK_CONTAINER(w->box_results));
    for (GList *r = old_rows; r; r = r->next)
        gtk_widget_destroy(GTK_WIDGET(r->data));
    g_list_free(old_rows);

    // 8) Poblar GtkListBox
    for (guint s = 0; s < sol_list->len; s++) {
        gboolean *mask = g_ptr_array_index(sol_list, s);
        GtkListBoxRow *row = GTK_LIST_BOX_ROW(
            gtk_list_box_row_new());
        GtkWidget *hbox = gtk_box_new(
            GTK_ORIENTATION_HORIZONTAL, 4);

        int suma = 0;
        for (int i = 0; i < n; i++) {
            if (mask[i]) suma += A[i];
            GtkWidget *chk = gtk_check_button_new();
            gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(chk), mask[i]);
            gtk_widget_set_sensitive(chk, FALSE);
            gtk_box_pack_start(GTK_BOX(hbox), chk,
                               FALSE, FALSE, 0);
        }

        GtkWidget *lbl_sum = gtk_label_new(
            g_strdup_printf(" Suma = %d", suma));
        gtk_box_pack_end(GTK_BOX(hbox), lbl_sum,
                         FALSE, FALSE, 4);

        gtk_container_add(GTK_CONTAINER(row), hbox);
        gtk_list_box_insert(GTK_LIST_BOX(w->box_results),
                            GTK_WIDGET(row), -1);
        gtk_widget_show_all(GTK_WIDGET(row));
    }

    // 9) Liberar memoria
    free(A);
    free(suffix_sum);
    suffix_sum = NULL;
    g_ptr_array_free(sol_list, TRUE);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new_from_file("interfaz.glade");
    AppWidgets *w = g_new0(AppWidgets, 1);

    // --- Recupera TODO lo que necesitas del builder ---
    // Panel izquierdo: a_i y Δ
    w->box_ai     = GTK_BOX(
        gtk_builder_get_object(builder, "box_ai"));
    w->box_delta  = GTK_BOX(
        gtk_builder_get_object(builder, "box_delta"));
    w->spin_delta = GTK_SPIN_BUTTON(
        gtk_builder_get_object(builder, "spin_delta"));
    w->rb_delta   = GTK_TOGGLE_BUTTON(
        gtk_builder_get_object(builder, "varianteDelta"));

    // Spin de tamaño para regenerar los a_i
    GtkSpinButton *spin_size = GTK_SPIN_BUTTON(
        gtk_builder_get_object(builder, "size"));

    // Spin de W
    w->spin_w      = GTK_SPIN_BUTTON(
        gtk_builder_get_object(builder, "spin_w"));

    // Radios de variantes
    w->rb_v3       = GTK_TOGGLE_BUTTON(
        gtk_builder_get_object(builder, "varianteMayorIgual"));
    w->rb_v4       = GTK_TOGGLE_BUTTON(
        gtk_builder_get_object(builder, "varianteMIAcotado"));

    // Resultados
    w->lbl_count   = GTK_LABEL(
        gtk_builder_get_object(builder, "lbl_count"));
    w->lbl_nodes   = GTK_LABEL(
        gtk_builder_get_object(builder, "lbl_nodes"));
    w->box_results = GTK_LIST_BOX(
        gtk_builder_get_object(builder, "list_solutions"));
        g_return_val_if_fail(GTK_IS_LIST_BOX(w->box_results), 1);

    // Botón Ejecutar y Botón Salir
    w->btn_execute = GTK_BUTTON(
        gtk_builder_get_object(builder, "btn_execute"));

    GtkButton *btn_exit = GTK_BUTTON(
        gtk_builder_get_object(builder, "buttonFinish"));
    
    gtk_list_box_set_activate_on_single_click(
        GTK_LIST_BOX(w->box_results), TRUE);
        
    // --- Conecta señales ---
    // Redibujar los a_i al cambiar el spin de tamaño
    g_signal_connect(spin_size, "value-changed",
                     G_CALLBACK(on_size_value_changed), w);
    // Mostrar/ocultar Δ
    g_signal_connect(w->rb_delta, "toggled",
                     G_CALLBACK(on_delta_toggled), w);
    // Pulsar ejecutar
    g_signal_connect(w->btn_execute, "clicked",
                     G_CALLBACK(on_execute_clicked), w);
    // Salir
    g_signal_connect(btn_exit, "clicked",
                     G_CALLBACK(gtk_main_quit), NULL);
    // Panel divisor (si lo usas para algo)
    GtkWidget *panel = GTK_WIDGET(gtk_builder_get_object(builder, "division"));
    g_signal_connect(panel, "notify::position", G_CALLBACK(fijar_panel), NULL);

    // --- Estado inicial de la UI ---
    // 1) Genera los a_i con el valor inicial de "size"
    on_size_value_changed(spin_size, w);

    // --- Muestra todo y fullscreen ---
    GtkWidget *window = GTK_WIDGET(
        gtk_builder_get_object(builder, "ventana"));
    gtk_widget_show_all(window);
    gtk_window_fullscreen(GTK_WINDOW(window));

    // 2) Oculta la caja de Δ si rb_delta no está activo
    on_delta_toggled(w->rb_delta, w);

    gtk_main();

    g_free(w);
    g_object_unref(builder);
    return 0;
}