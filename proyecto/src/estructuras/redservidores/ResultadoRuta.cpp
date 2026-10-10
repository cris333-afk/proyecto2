// Responsabilidad: guardar el resultado de Dijkstra con memoria propia.
// Depende de: ResultadoRuta.h.
#include "ResultadoRuta.h"

// Crea un resultado vacío sin ruta.
ResultadoRuta::ResultadoRuta() {
    costoTotal = -1;
    ids = nullptr;
    largoRuta = 0;
    hayRuta = false;
}

// Libera el arreglo de ids si existe.
ResultadoRuta::~ResultadoRuta() {
    delete[] ids;
}

// Guarda el costo y copia los ids de la ruta encontrada.
void ResultadoRuta::fijar(int costo, const int* origen, int largo) {
    // Se libera lo anterior porque el objeto es dueño único del arreglo.
    delete[] ids;
    ids = nullptr;
    costoTotal = costo;
    largoRuta = largo;
    hayRuta = true;
    if (largoRuta > 0) {
        ids = new int[largoRuta];
        for (int i = 0; i < largoRuta; ++i) {
            ids[i] = origen[i];
        }
    }
}

// Marca que no existe camino entre origen y destino.
void ResultadoRuta::marcarSinRuta() {
    delete[] ids;
    ids = nullptr;
    costoTotal = -1;
    largoRuta = 0;
    hayRuta = false;
}

// Dice si se encontró un camino.
bool ResultadoRuta::existe() const {
    return hayRuta;
}

// Devuelve el costo total en ms, o -1 si no hay ruta.
int ResultadoRuta::costo() const {
    return costoTotal;
}

// Devuelve cuántos nodos tiene la ruta.
int ResultadoRuta::largo() const {
    return largoRuta;
}

// Devuelve el id en la posición i (de origen a destino).
int ResultadoRuta::idEn(int i) const {
    return ids[i];
}
