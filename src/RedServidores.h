#ifndef RED_SERVIDORES_H
#define RED_SERVIDORES_H

#include <string>

// Nodo de lista de adyacencia: una conexión hacia otro servidor.
struct Arista {
    int destino;
    int latenciaMs;
    Arista* siguiente;
};

// Grafo ponderado no dirigido con listas de adyacencia manuales.
class RedServidores {
public:
    // Reserva los arreglos de cabezas y nombres.
    explicit RedServidores(int maxServidores = 20);
    // Libera todas las aristas y luego los arreglos.
    ~RedServidores();

    // Agrega un servidor y devuelve su id, o -1 si no cabe o el nombre es vacío.
    int agregarServidor(const std::string& nombre);
    // Agrega la arista en ambos sentidos, o false si los datos no son válidos.
    bool agregarConexion(int a, int b, int latenciaMs);
    // Quita la arista de ambas listas, o false si no existía.
    bool eliminarConexion(int a, int b);
    // Calcula la ruta más corta (Dijkstra O(V^2)).
    int rutaMasCorta(int origen, int destino);
    // Revisa con BFS si la red es conexa desde el servidor 0.
    bool pingGeneral();
    // Devuelve el nombre del servidor, o "" si el id es inválido.
    std::string nombreDe(int id) const;

private:
    Arista** adyacencia;      // cabezas de listas de adyacencia por id
    std::string* nombres;     // nombre del servidor por id
    int cantidad;             // servidores agregados hasta ahora
    int maxServidores;        // capacidad reservada con new[]

    // Dice si el id está dentro del rango de servidores agregados.
    bool idValido(int id) const;
    // Dice si ya existe la conexión entre a y b.
    bool existeConexion(int a, int b) const;
    // Busca la latencia del enlace origen -> destino, o -1 si no existe.
    int latenciaEntre(int a, int b) const;
    // Borra el enlace origen -> destino de una sola lista.
    bool eliminarUnSentido(int origen, int destino);

    // Se prohíbe copiar para no duplicar punteros y evitar doble delete.
    RedServidores(const RedServidores&) = delete;
    RedServidores& operator=(const RedServidores&) = delete;
};

#endif
