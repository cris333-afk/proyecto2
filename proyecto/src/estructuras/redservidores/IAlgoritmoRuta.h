// Responsabilidad: interfaz del algoritmo de ruta mínima para cambiarlo sin tocar RedServidores.
// Depende de: IGrafo.h y ResultadoRuta.h.
#ifndef IALGORITMO_RUTA_H
#define IALGORITMO_RUTA_H

#include "IGrafo.h"
#include "ResultadoRuta.h"

// Contrato para calcular la ruta más corta sobre un grafo de lectura.
class IAlgoritmoRuta {
public:
    // Libera recursos de la interfaz (no hace nada por sí misma).
    virtual ~IAlgoritmoRuta() {}
    // Calcula la ruta y deja el resultado en salida.
    virtual void calcular(const IGrafo& grafo, int origen, int destino, ResultadoRuta& salida) = 0;
};

#endif
