typedef struct {
    /* sección A_i y ∆ ya existente */
    GtkBox *box_ai;
    GtkBox *box_delta;
    GtkSpinButton *spin_delta;
    GtkToggleButton *rb_delta;

    /* nuevo: W */
    GtkSpinButton *spin_w;

    /* radios de variantes */
    GtkToggleButton *rb_v3;    // “Mayor o Igual”
    GtkToggleButton *rb_v4;    // “Mayor o Igual Acotado”

    /* resultados */
    GtkLabel *lbl_count;
    GtkLabel *lbl_nodes;
    GtkListBox *box_results;  // el GtkBox dentro del Viewport/ScrolledWindow

    /* botón “Ejecutar” */
    GtkButton *btn_execute;
} AppWidgets;
