/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Grafo — Recorridos (BFS / DFS)
 * Archivo     : recorridos.cpp
 * Descripción : Implementación del recorrido BFS por niveles.
 * Autor       : Angel
 * ========================================================================== */

// Responsabilidad: delegar el BFS en BusquedaAnchura para no duplicar lógica.
// Depende de: recorridos.hpp y BusquedaAnchura.h.
#include "recorridos.hpp"
#include "../redservidores/BusquedaAnchura.h"

// Recorre desde origen y devuelve visitado con new[] (o nullptr si falla).
// El llamador es dueño del arreglo y debe liberarlo con delete[].
bool* bfs(const IGrafo& grafo, int origen) {
    return BusquedaAnchura::recorrer(grafo, origen);
}
