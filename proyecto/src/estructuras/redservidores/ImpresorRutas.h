// Responsabilidad: imprimir rutas y diagnósticos con el formato exacto del módulo.
// Depende de: IGrafo.h y ResultadoRuta.h, solo iostream.
#ifndef IMPRESOR_RUTAS_H
#define IMPRESOR_RUTAS_H

#include "IGrafo.h"
#include "ResultadoRuta.h"

// Imprime en pantalla sin calcular nada ni registrar auditoría.
class ImpresorRutas {
public:
    // Imprime la ruta con sus saltos y el costo total.
    static void imprimirRuta(const IGrafo& grafo, const ResultadoRuta& ruta);
    // Imprime que no existe ruta entre los servidores indicados.
    static void imprimirSinRuta();
    // Imprime que los ids de servidor son inválidos.
    static void imprimirIdsInvalidos();
    // Imprime que no hay servidores en la red.
    static void imprimirRedVacia();
    // Imprime que toda la red es alcanzable.
    static void imprimirRedConexa();
    // Imprime los no alcanzados marcando AISLADO o RED PARTIDA.
    static void imprimirNoAlcanzados(const IGrafo& grafo, const bool* visitado, int total);
};

#endif
