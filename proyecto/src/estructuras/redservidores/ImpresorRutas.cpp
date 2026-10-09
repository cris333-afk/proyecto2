// Responsabilidad: imprimir rutas y diagnósticos con el formato exacto del módulo.
// Depende de: ImpresorRutas.h, IGrafo.h y ResultadoRuta.h.
#include "ImpresorRutas.h"

#include <iostream>

// Imprime la ruta con sus saltos y el costo total.
void ImpresorRutas::imprimirRuta(const IGrafo& grafo, const ResultadoRuta& ruta) {
    std::cout << "Ruta:";
    for (int i = 0; i < ruta.largo(); ++i) {
        std::cout << " " << grafo.nombreDe(ruta.idEn(i));
        if (i + 1 < ruta.largo()) {
            std::cout << " ->";
        }
    }
    std::cout << std::endl;
    for (int i = 0; i + 1 < ruta.largo(); ++i) {
        // La latencia se lee del grafo porque el resultado solo guarda ids.
        int latencia = -1;
        const Arista* arista = grafo.primeraArista(ruta.idEn(i));
        while (arista != nullptr) {
            if (arista->destino == ruta.idEn(i + 1)) {
                latencia = arista->latenciaMs;
                break;
            }
            arista = arista->siguiente;
        }
        std::cout << "  " << grafo.nombreDe(ruta.idEn(i)) << " -> " << grafo.nombreDe(ruta.idEn(i + 1)) << " : " << latencia << " ms" << std::endl;
    }
    std::cout << "Costo total: " << ruta.costo() << " ms" << std::endl;
}

// Imprime que no existe ruta entre los servidores indicados.
void ImpresorRutas::imprimirSinRuta() {
    std::cout << "No existe ruta entre los servidores indicados." << std::endl;
}

// Imprime que los ids de servidor son inválidos.
void ImpresorRutas::imprimirIdsInvalidos() {
    std::cout << "Ruta no calculada: ids de servidor invalidos." << std::endl;
}

// Imprime que no hay servidores en la red.
void ImpresorRutas::imprimirRedVacia() {
    std::cout << "Ping general: no hay servidores en la red." << std::endl;
}

// Imprime que toda la red es alcanzable.
void ImpresorRutas::imprimirRedConexa() {
    std::cout << "Red conexa: todos los servidores son alcanzables." << std::endl;
}

// Imprime los no alcanzados marcando AISLADO o RED PARTIDA.
void ImpresorRutas::imprimirNoAlcanzados(const IGrafo& grafo, const bool* visitado, int total) {
    std::cout << "Servidores no alcanzados:" << std::endl;
    for (int i = 0; i < total; ++i) {
        if (!visitado[i]) {
            // Cabeza en nullptr significa que ahora no tiene conexiones.
            if (grafo.primeraArista(i) == nullptr) {
                std::cout << "  " << grafo.nombreDe(i) << " (id " << i << "): AISLADO" << std::endl;
            } else {
                std::cout << "  " << grafo.nombreDe(i) << " (id " << i << "): RED PARTIDA" << std::endl;
            }
        }
    }
}
