// Responsabilidad: cola de enteros reutilizable para el BFS.
// Depende de: ColaCircular.h.
#include "ColaCircular.h"

// Reserva el arreglo con la capacidad indicada.
ColaCircular::ColaCircular(int capacidad) {
    // Se fija un mínimo válido porque new[] exige un tamaño positivo.
    if (capacidad <= 0) {
        capacidad = 1;
    }
    this->capacidad = capacidad;
    datos = new int[this->capacidad];
    frente = 0;
    final = 0;
    elementos = 0;
}

// Libera el arreglo reservado.
ColaCircular::~ColaCircular() {
    delete[] datos;
}

// Agrega un valor al final, o false si está llena.
bool ColaCircular::encolar(int valor) {
    // Llena no admite más elementos sin sobrescribir el frente.
    if (elementos >= capacidad) {
        return false;
    }
    datos[final] = valor;
    // El módulo hace circular el índice para reutilizar el espacio liberado.
    final = (final + 1) % capacidad;
    ++elementos;
    return true;
}

// Saca el valor del frente, o false si está vacía.
bool ColaCircular::sacar(int& valor) {
    if (elementos <= 0) {
        return false;
    }
    valor = datos[frente];
    // El módulo avanza el frente sin desplazar los demás elementos.
    frente = (frente + 1) % capacidad;
    --elementos;
    return true;
}

// Dice si no hay elementos.
bool ColaCircular::vacia() const {
    return elementos == 0;
}
