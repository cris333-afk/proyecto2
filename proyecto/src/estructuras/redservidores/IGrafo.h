// Responsabilidad: interfaz mínima de lectura del grafo para los algoritmos.
// Depende de: solo <string>, sin contenedores STL.
#ifndef IGRAFO_H
#define IGRAFO_H

#include <string>

// Nodo de lista de adyacencia: una conexión hacia otro servidor.
struct Arista {
    int destino;
    int latenciaMs;
    Arista* siguiente;
};

// Vista de solo lectura para que Dijkstra/BFS no dependan de RedServidores.
class IGrafo {
public:
    // Libera recursos de la interfaz (no hace nada por sí misma).
    virtual ~IGrafo() {}
    // Devuelve cuántos servidores hay agregados.
    virtual int cantidadServidores() const = 0;
    // Devuelve la cabeza de la lista de un servidor, o nullptr si no tiene.
    virtual const Arista* primeraArista(int id) const = 0;
    // Devuelve el nombre del servidor, o "" si el id es inválido.
    virtual std::string nombreDe(int id) const = 0;
    // Dice si el id está dentro del rango de servidores agregados.
    virtual bool idValido(int id) const = 0;
};

#endif
