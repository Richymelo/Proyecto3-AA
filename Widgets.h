/*
                Esta sección contiene una estructura con la mayoría de
                Widgets con los que el usuario puede interactuar en el
                programa. Están los widgets de la sección de números ai
                del conjunto, los widgets que controlan el número delta,
                el widget de w (solución que se quiere), los widgets de
                las 4 variantes de algoritmos y los widgets para los
                resultados.
*/
typedef struct {
    /* sección A_i y ∆ */
    GtkBox *box_ai;
    GtkBox *box_delta;
    GtkSpinButton *spin_delta;
    GtkToggleButton *rb_delta;

    /* W */
    GtkSpinButton *spin_w;

    /* Radios de variantes */
    GtkToggleButton *rb_v1;    // “Básica”
    GtkToggleButton *rb_v2;    // “Delta”
    GtkToggleButton *rb_v3;    // “Mayor o Igual”
    GtkToggleButton *rb_v4;    // “Mayor o Igual Acotado”

    /* Resultados */
    GtkLabel *lbl_count;
    GtkLabel *lbl_nodes;
    GtkListBox *box_results;

    /* Botón “Ejecutar” */
    GtkButton *btn_execute;
} AppWidgets;
