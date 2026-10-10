// Responsabilidad: cola de enteros reutilizable para el BFS.
// Depende de: nada, solo un arreglo con new[]/delete[].
#ifndef COLA_CIRCULAR_H
#define COLA_CIRCULAR_H

// Cola circular que reutiliza el espacio sin desplazar elementos.
class ColaCircular {
public:
    // Reserva el arreglo con la capacidad indicada.
    explicit ColaCircular(int capacidad);
    // Libera el arreglo reservado.
    ~ColaCircular();

    // Agrega un valor al final, o false si está llena.
    bool encolar(int valor);
    // Saca el valor del frente, o false si está vacía.
    bool sacar(int& valor);
    // Dice si no hay elementos.
    bool vacia() const;

private:
    int* datos;      // arreglo circular con new[]
    int capacidad;   // tamaño reservado
    int frente;      // índice del próximo a sacar
    int final;       // índice del próximo espacio libre
    int elementos;   // cantidad actual en la cola

    // Se prohíbe copiar para no duplicar el arreglo y evitar doble delete.
    ColaCircular(const ColaCircular&) = delete;
    ColaCircular& operator=(const ColaCircular&) = delete;
};

#endif
