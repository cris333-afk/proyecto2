/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Grafo / Red — pruebas + estrés
 * Archivo     : tests/test_grafo.cpp
 * Descripción : Suite de RedServidores modular: red de 100 servidores con
 *               conexiones pseudoaleatorias (semilla fija, sin STL), Dijkstra
 *               en varios pares + par sin ruta, 500 agrega/elimina y ping BFS
 *               con aislado. Solo usa la API pública, sin tocar algoritmos.
 * Compilar (parado en proyecto/):
 *   g++ -std=c++17 -Wall -Wextra -g -fsanitize=address -Isrc -o tests/output/test_grafo.exe tests/test_grafo.cpp src/estructuras/redservidores/RedServidores.cpp src/estructuras/redservidores/ResultadoRuta.cpp src/estructuras/redservidores/ColaCircular.cpp src/estructuras/redservidores/BusquedaAnchura.cpp src/estructuras/redservidores/ImpresorRutas.cpp src/estructuras/grafo/dijkstra.cpp src/estructuras/grafo/recorridos.cpp src/auditoria/auditoria.cpp
 * ========================================================================== */

#include <iostream>
#include <string>

#include "../src/estructuras/redservidores/RedServidores.h"

static int gOk = 0;
static int gFail = 0;

static void verificar(bool c, const std::string& d) {
    if (c) { ++gOk; std::cout << "  [OK]    " << d << "\n"; }
    else { ++gFail; std::cout << "  [FALLO] " << d << "\n"; }
}

// Pseudoaleatorio determinista sin <random>: LCG propio.
static unsigned int gSemilla = 12345;
static unsigned int siguienteAzar() {
    gSemilla = gSemilla * 1103515245 + 12345;
    return (gSemilla / 65536) % 32768;
}

int main() {
    std::cout << "===== PRUEBAS RED / GRAFO (100 nodos + estres) =====\n";

    RedServidores red(120);
    for (int i = 0; i < 100; ++i) {
        int id = red.agregarServidor("srv_" + std::to_string(i));
        if (id != i) verificar(false, "ids secuenciales al agregar 100");
    }
    verificar(red.cantidadServidores() == 100, "100 servidores agregados");

    // ~300 conexiones aleatorias deterministas (origen<->destino, lat 1..50).
    int creadas = 0;
    for (int k = 0; k < 300; ++k) {
        int a = (int)(siguienteAzar() % 99);  // 0..98, el 99 queda aislado
        int b = (int)(siguienteAzar() % 99);
        int lat = 1 + (int)(siguienteAzar() % 50);
        if (a != b && red.agregarConexion(a, b, lat)) ++creadas;
    }
    verificar(creadas > 200, "más de 200 conexiones creadas en malla 0..98");

    // Dijkstra en varios pares: costo >= 0 y ruta impresa por el módulo.
    int c1 = red.rutaMasCorta(0, 10);
    int c2 = red.rutaMasCorta(5, 50);
    int c3 = red.rutaMasCorta(20, 80);
    verificar(c1 >= 0 && c2 >= 0 && c3 >= 0, "Dijkstra halla ruta en 3 pares de la malla");

    // Par sin ruta: el 99 está aislado (nunca se conectó).
    verificar(red.primeraArista(99) == nullptr, "srv_99 aislado sin aristas");
    verificar(red.rutaMasCorta(0, 99) == -1, "Dijkstra devuelve -1 a nodo inalcanzable");

    // 500 operaciones agrega/elimina repetidas sobre el mismo par.
    bool corrupta = false;
    for (int k = 0; k < 500; ++k) {
        red.eliminarConexion(1, 2);
        if (!red.agregarConexion(1, 2, 5 + (k % 10))) { corrupta = true; break; }
        if (k % 2 == 0) red.eliminarConexion(1, 2);
    }
    // Tras el ciclo la lista de 1 debe recorrerse sin ciclos infinitos.
    const Arista* p = red.primeraArista(1);
    int pasos = 0;
    while (p != nullptr && pasos < 500) { p = p->siguiente; ++pasos; }
    verificar(!corrupta && pasos < 500, "500 agrega/elimina sin corromper adyacencia");
    red.eliminarConexion(1, 2);

    // Ping general debe detectar al aislado 99.
    verificar(red.pingGeneral() == false, "ping BFS detecta red partida por aislado 99");

    std::cout << "\n===== RESUMEN GRAFO =====\nOK: " << gOk << " FALLOS: " << gFail << "\n";
    return 0;
}
