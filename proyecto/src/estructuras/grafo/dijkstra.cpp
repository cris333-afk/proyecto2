/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Grafo — Dijkstra
 * Archivo     : dijkstra.cpp
 * Descripción : Implementación del algoritmo de Dijkstra.
 * Autor       : Angel
 * ========================================================================== */

// Responsabilidad: calcular la ruta mínima con Dijkstra O(V^2) sin imprimir ni auditar.
// Depende de: dijkstra.hpp y la interfaz IGrafo.
#include "dijkstra.hpp"

// Calcula la ruta y deja el resultado en salida.
// No admite pesos negativos porque asume que el nodo extraído ya es final;
// su complejidad es O(V^2) por elegir el mínimo con un recorrido lineal.
void Dijkstra::calcular(const IGrafo& grafo, int origen, int destino, ResultadoRuta& salida) {
    salida.marcarSinRuta();
    // Solo se aceptan ids de servidores ya agregados.
    if (!grafo.idValido(origen) || !grafo.idValido(destino)) {
        return;
    }
    int total = grafo.cantidadServidores();
    if (total <= 0) {
        return;
    }
    // El costo hacia uno mismo es cero y no requiere recorrer la red.
    if (origen == destino) {
        int unico = origen;
        salida.fijar(0, &unico, 1);
        return;
    }
    // Se reserva el estado de Dijkstra con new[] por la regla del curso.
    const int INF = 1000000000;
    int* costos = new int[total];
    int* predecesor = new int[total];
    bool* visitado = new bool[total];
    for (int i = 0; i < total; ++i) {
        costos[i] = INF;
        predecesor[i] = -1;
        visitado[i] = false;
    }
    costos[origen] = 0;
    // Se fija un nodo por vuelta hasta visitar todos los alcanzables.
    for (int vuelta = 0; vuelta < total; ++vuelta) {
        // Se elige con recorrido lineal el no visitado de menor costo.
        int u = -1;
        int menor = INF;
        for (int i = 0; i < total; ++i) {
            if (!visitado[i] && costos[i] < menor) {
                menor = costos[i];
                u = i;
            }
        }
        // Si el menor sigue en INF ya no quedan nodos alcanzables.
        if (u == -1) {
            break;
        }
        visitado[u] = true;
        // Se relajan las aristas que salen del nodo recién fijado.
        const Arista* arista = grafo.primeraArista(u);
        while (arista != nullptr) {
            int v = arista->destino;
            int w = arista->latenciaMs;
            // Solo se relaja con costo conocido para evitar desbordamiento.
            if (grafo.idValido(v) && costos[u] != INF && w >= 0 && costos[u] <= INF - w && costos[u] + w < costos[v]) {
                costos[v] = costos[u] + w;
                predecesor[v] = u;
            }
            arista = arista->siguiente;
        }
    }
    // Si el destino quedó en INF es porque no hay camino.
    if (costos[destino] == INF) {
        delete[] costos;
        delete[] predecesor;
        delete[] visitado;
        return;
    }
    int costoTotal = costos[destino];
    // Se reconstruye la ruta desde el destino hasta el origen.
    int* invertida = new int[total];
    int largo = 0;
    int actual = destino;
    // Se sigue el predecesor porque cada nodo guarda por dónde se llegó.
    while (actual != -1 && largo < total) {
        invertida[largo] = actual;
        ++largo;
        actual = predecesor[actual];
    }
    // Se invierte porque quedó de destino a origen.
    int* ordenada = new int[largo];
    for (int i = 0; i < largo; ++i) {
        ordenada[i] = invertida[largo - 1 - i];
    }
    salida.fijar(costoTotal, ordenada, largo);
    // Se liberan todos los arreglos temporales en la salida normal.
    delete[] ordenada;
    delete[] invertida;
    delete[] costos;
    delete[] predecesor;
    delete[] visitado;
}
