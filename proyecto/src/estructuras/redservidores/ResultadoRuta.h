// Responsabilidad: guardar el resultado de Dijkstra con memoria propia.
// Depende de: nada, solo arreglos con new[]/delete[].
#ifndef RESULTADO_RUTA_H
#define RESULTADO_RUTA_H

// Guarda costo, ids de la ruta y si existe camino.
class ResultadoRuta {
public:
    // Crea un resultado vacío sin ruta.
    ResultadoRuta();
    // Libera el arreglo de ids si existe.
    ~ResultadoRuta();

    // Guarda el costo y copia los ids de la ruta encontrada.
    void fijar(int costo, const int* ids, int largo);
    // Marca que no existe camino entre origen y destino.
    void marcarSinRuta();
    // Dice si se encontró un camino.
    bool existe() const;
    // Devuelve el costo total en ms, o -1 si no hay ruta.
    int costo() const;
    // Devuelve cuántos nodos tiene la ruta.
    int largo() const;
    // Devuelve el id en la posición i (de origen a destino).
    int idEn(int i) const;

private:
    int costoTotal;   // costo mínimo, -1 si no hay ruta
    int* ids;         // ids de origen a destino con new[]
    int largoRuta;    // cantidad de nodos en ids
    bool hayRuta;     // true cuando fijar() guardó un camino

    // Se prohíbe copiar para no duplicar el arreglo y evitar doble delete.
    ResultadoRuta(const ResultadoRuta&) = delete;
    ResultadoRuta& operator=(const ResultadoRuta&) = delete;
};

#endif
