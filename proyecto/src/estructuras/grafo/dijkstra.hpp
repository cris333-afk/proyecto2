/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Grafo — Dijkstra
 * Archivo     : dijkstra.hpp
 * Descripción : Declaración del cálculo de rutas mínimas (algoritmo de
 *               Dijkstra) sobre el grafo ponderado de la red.
 * Autor       : Angel
 * ========================================================================== */

// Responsabilidad: calcular la ruta mínima con Dijkstra O(V^2) sin imprimir ni auditar.
// Depende de: IGrafo.h, IAlgoritmoRuta.h y ResultadoRuta.h.
#ifndef PROYECTO_DIJKSTRA_HPP
#define PROYECTO_DIJKSTRA_HPP

#include "../redservidores/IAlgoritmoRuta.h"
#include "../redservidores/IGrafo.h"
#include "../redservidores/ResultadoRuta.h"

// Dijkstra que trabaja contra la interfaz para no depender de RedServidores.
class Dijkstra : public IAlgoritmoRuta {
public:
    // Calcula la ruta y deja el resultado en salida.
    void calcular(const IGrafo& grafo, int origen, int destino, ResultadoRuta& salida) override;
};

#endif // PROYECTO_DIJKSTRA_HPP

