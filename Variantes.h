extern int nodos, soluciones;
extern int delta;
static int *suffix_sum;

// ========================
// VARIANTE 1: Basica
// ========================
void
sumaSubconjuntosV1_collect(int* A, int n, int W,
                           int index,
                           int* actual_idx,
                           int tam_actual,
                           int suma_actual,
                           GPtrArray *sol_list)
{
    extern int nodos, soluciones;
    nodos++;

    // 1) Poda si ya pasé W
    if (suma_actual > W)
        return;

    // 2) Bounding: si ni con todo lo que queda llego a W
    if (suma_actual + suffix_sum[index] < W)
        return;

    // 3) Si llegué al final (hoja)
    if (index == n) {
        if (suma_actual == W) {
            soluciones++;
            gboolean *mask = g_new0(gboolean, n);
            for (int j = 0; j < tam_actual; j++)
                mask[ actual_idx[j] ] = TRUE;
            g_ptr_array_add(sol_list, mask);
        }
        return;
    }

    // 4) Rama “incluir A[index]” — ¡solo si suma_actual + A[index] ≤ W!
    if (suma_actual + A[index] <= W) {
        actual_idx[tam_actual] = index;
        sumaSubconjuntosV1_collect(
            A, n, W,
            index + 1,
            actual_idx, tam_actual + 1,
            suma_actual + A[index],
            sol_list
        );
    }

    // 5) Rama “excluir A[index]”
    sumaSubconjuntosV1_collect(
        A, n, W,
        index + 1,
        actual_idx, tam_actual,
        suma_actual,
        sol_list
    );
}
// ========================
// VARIANTE 2: Delta
// ========================
void
sumaSubconjuntosV2_collect(int* A, int n, int W,
    int index,
    int* actual_idx,
    int tam_actual,
    int suma_actual,
    GPtrArray *sol_list)
{
    extern int nodos, soluciones;
    nodos++;
    // 0) Bounding superior: si ya pasé W+Δ ⇒ podo
    if (suma_actual > W + delta)
        return;

    // 1) Bounding inferior: si ni con todo lo que queda llego a W-Δ ⇒ podo
    if (index < n &&
        suma_actual + suffix_sum[index] < W - delta)
    {
        return;
    }

    // 2) Si la suma está ya en [W-Δ, W+Δ], registro y **corto** la rama
    if (suma_actual >= W - delta &&
        suma_actual <= W + delta)
    {
        soluciones++;
        gboolean *mask = g_new0(gboolean, n);
        for (int j = 0; j < tam_actual; j++)
            mask[ actual_idx[j] ] = TRUE;
        g_ptr_array_add(sol_list, mask);
        return;  // <<-- poda aquí para no explorar supersets
    }

    // 3) Si llegué al final, no tengo nada más que hacer
    if (index == n)
        return;

    // 4) Rama “incluir A[index]”
    actual_idx[tam_actual] = index;
    sumaSubconjuntosV2_collect(
        A, n, W,
        index + 1,
        actual_idx, tam_actual + 1,
        suma_actual + A[index],
        sol_list
    );

    // 5) Rama “excluir A[index]”
    sumaSubconjuntosV2_collect(
        A, n, W,
        index + 1,
        actual_idx, tam_actual,
        suma_actual,
        sol_list
    );
}
// ========================
// VARIANTE 3: Mayor o Igual
// ========================
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

// ====================================
// VARIANTE 4: Mayor o Igual Acotado
// ====================================
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