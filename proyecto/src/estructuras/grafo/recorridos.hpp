/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Grafo — Recorridos (BFS / DFS)
 * Archivo     : recorridos.hpp
 * Descripción : Recorrido BFS por niveles sobre la interfaz del grafo.
 * Autor       : Angel
 * ========================================================================== */

// Responsabilidad: exponer el BFS que usa el ping sin atarse a RedServidores.
// Depende de: IGrafo.h de redservidores.
#ifndef PROYECTO_RECORRIDOS_HPP
#define PROYECTO_RECORRIDOS_HPP

#include "../redservidores/IGrafo.h"

// Recorre desde origen y devuelve visitado con new[] (o nullptr si falla).
// El llamador es dueño del arreglo y debe liberarlo con delete[].
bool* bfs(const IGrafo& grafo, int origen);

#endif // PROYECTO_RECORRIDOS_HPP

