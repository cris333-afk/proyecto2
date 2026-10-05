// Responsabilidad: recorrer la red por niveles con BFS y devolver los alcanzados.
// Depende de: BusquedaAnchura.h, ColaCircular.h e IGrafo.h.
#include "BusquedaAnchura.h"
#include "ColaCircular.h"

// Recorre desde origen y devuelve visitado con new[] (o nullptr si falla).
// El llamador es dueño del arreglo y debe liberarlo con delete[].
bool* BusquedaAnchura::recorrer(const IGrafo& grafo, int origen) {
    int total = grafo.cantidadServidores();
    // Sin nodos u origen inválido no hay recorrido que hacer.
    if (total <= 0 || !grafo.idValido(origen)) {
        return nullptr;
    }
    bool* visitado = new bool[total];
    for (int i = 0; i < total; ++i) {
        visitado[i] = false;
    }
    ColaCircular cola(total);
    // Se encola el origen y se marca para no visitarlo dos veces.
    cola.encolar(origen);
    visitado[origen] = true;
    int actual = origen;
    // Se usa BFS y no DFS porque recorre por niveles, ideal para el ping.
    while (!cola.vacia()) {
        cola.sacar(actual);
        const Arista* arista = grafo.primeraArista(actual);
        while (arista != nullptr) {
            int vecino = arista->destino;
            // Solo se encolan vecinos nuevos para no repetir visitas.
            if (grafo.idValido(vecino) && !visitado[vecino]) {
                visitado[vecino] = true;
                cola.encolar(vecino);
            }
            arista = arista->siguiente;
        }
    }
    return visitado;
}
