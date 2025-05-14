# Proyecto3-AA
#Suma de Subconjuntos

**Lenguaje:** C  
**Entorno:** Linux 
**Interfaz Gráfica:** GTK + Glade  

---

## Descripción General

Este proyecto resuelve el problema clásico de **Suma de Subconjuntos** mediante algoritmos de **backtracking**, implementando cuatro variantes del problema. El usuario ingresa un conjunto de números enteros positivos y una meta `W`, y el programa encuentra subconjuntos cuya suma cumpla con ciertos criterios.

---

## Variantes Implementadas

1. **Básica**: suma exactamente igual a `W`.
2. **Delta (∆)**: suma dentro del rango `[W - ∆, W + ∆]`.
3. **Mayor o Igual**: suma mayor o igual a `W`, permitiendo continuar la búsqueda.
4. **Mayor o Igual Acotado**: suma mayor o igual a `W`, deteniéndose al encontrar una solución válida.
