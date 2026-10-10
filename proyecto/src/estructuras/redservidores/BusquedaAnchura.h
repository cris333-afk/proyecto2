// Responsabilidad: recorrer la red por niveles con BFS y devolver los alcanzados.
// Depende de: IGrafo.h y ColaCircular.h.
#ifndef BUSQUEDA_ANCHURA_H
#define BUSQUEDA_ANCHURA_H

#include "IGrafo.h"

// Recorre desde un origen y crea el arreglo de visitados con new[].
class BusquedaAnchura {
public:
    // Recorre desde origen y devuelve visitado con new[] (o nullptr si falla).
    // El llamador es dueño del arreglo y debe liberarlo con delete[].
    static bool* recorrer(const IGrafo& grafo, int origen);
};

#endif
