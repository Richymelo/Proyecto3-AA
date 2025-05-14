extern int nodos, soluciones;
static int *suffix_sum;  // inicializado en main

void sumaSubconjuntosV3_collect(int* A, int n, int W,
    int index, int* actual_idx,
    int tam_actual, int suma_actual,
    GPtrArray *sol_list)
{
    extern int nodos, soluciones;
    nodos++;

    // 0) Bounding: si ni siquiera con todo lo que queda llego a W…
    if (suma_actual + suffix_sum[index] < W) {
        return;
    }

    // 1) Si llegué al final, estoy en una hoja
    if (index == n) {
        if (suma_actual >= W) {
            soluciones++;
            gboolean *mask = g_new0(gboolean, n);
            for (int j = 0; j < tam_actual; j++)
                mask[ actual_idx[j] ] = TRUE;
            g_ptr_array_add(sol_list, mask);
        }
        return;
    }

    // 2) Rama “incluir A[index]”
    actual_idx[tam_actual] = index;
    sumaSubconjuntosV3_collect(
        A, n, W,
        index + 1, actual_idx, tam_actual + 1,
        suma_actual + A[index], sol_list
    );

    // 3) Rama “excluir A[index]”
    sumaSubconjuntosV3_collect(
        A, n, W,
        index + 1, actual_idx, tam_actual,
        suma_actual, sol_list
    );
}

// Variante 4: Mayor o Igual Acotado (corta rama al alcanzar W).
void sumaSubconjuntosV4_collect(int* A, int n, int W,
    int index, int* actual_idx,
    int tam_actual, int suma_actual,
    GPtrArray *sol_list)
{
    extern int nodos, soluciones;
    nodos++;

    // 0) Bounding: si ni sumando todo lo que queda llego a W, podo
    if (suma_actual + suffix_sum[index] < W)
        return;

    // 1) Si ya alcancé la meta, grabo y podo
    if (suma_actual >= W) {
        soluciones++;
        gboolean *mask = g_new0(gboolean, n);
        for (int j = 0; j < tam_actual; j++)
            mask[ actual_idx[j] ] = TRUE;
        g_ptr_array_add(sol_list, mask);
        return;
    }

    // 2) Si procesé todos los elementos, paro
    if (index == n)
        return;

    // 3) Rama “incluir A[index]”
    actual_idx[tam_actual] = index;
    sumaSubconjuntosV4_collect(
        A, n, W,
        index + 1, actual_idx, tam_actual + 1,
        suma_actual + A[index],
        sol_list
    );

    // 4) Rama “excluir A[index]”
    sumaSubconjuntosV4_collect(
        A, n, W,
        index + 1, actual_idx, tam_actual,
        suma_actual,
        sol_list
    );
}