// Responsabilidad: guardar la topología del grafo y coordinar algoritmos, impresión y auditoría.
// Depende de: RedServidores.h, BusquedaAnchura, ImpresorRutas, Dijkstra y auditoría.
#include "RedServidores.h"

#include "BusquedaAnchura.h"
#include "ImpresorRutas.h"
#include "../grafo/dijkstra.hpp"
#include "../../auditoria/auditoria.hpp"

// Auditoría real del equipo (Cris): escribe en network_audit_log.txt.
#include <iostream>

// Reserva los arreglos de cabezas y nombres.
RedServidores::RedServidores(int maxServidores) {
    // Se fija un mínimo válido porque new[] exige un tamaño positivo.
    if (maxServidores <= 0) {
        maxServidores = 20;
    }
    this->maxServidores = maxServidores;
    cantidad = 0;
    adyacencia = new Arista*[this->maxServidores];
    nombres = new std::string[this->maxServidores];
    // Toda cabeza inicia en nullptr para saber que la lista está vacía.
    for (int i = 0; i < this->maxServidores; ++i) {
        adyacencia[i] = nullptr;
    }
}

// Libera todas las aristas y luego los arreglos.
RedServidores::~RedServidores() {
    // Se recorre cada lista para no dejar aristas sin liberar.
    for (int i = 0; i < maxServidores; ++i) {
        Arista* actual = adyacencia[i];
        while (actual != nullptr) {
            Arista* borrar = actual;
            actual = actual->siguiente;
            delete borrar;
        }
    }
    delete[] adyacencia;
    delete[] nombres;
}

// Agrega un servidor y devuelve su id, o -1 si no cabe o el nombre es vacío.
int RedServidores::agregarServidor(const std::string& nombre) {
    // Se rechaza el nombre vacío porque el servidor quedaría sin identidad.
    if (nombre.empty()) {
        return -1;
    }
    // Sin espacio no se puede asignar un id nuevo.
    if (cantidad >= maxServidores) {
        return -1;
    }
    int id = cantidad;
    nombres[id] = nombre;
    // La cabeza ya quedó en nullptr desde el constructor.
    ++cantidad;
    AuditLogger::registrar("Servidor agregado: id=" + std::to_string(id) + " nombre=" + nombre);
    return id;
}

// Devuelve el nombre del servidor, o "" si el id es inválido.
std::string RedServidores::nombreDe(int id) const {
    if (!idValido(id)) {
        return "";
    }
    return nombres[id];
}

// Devuelve cuántos servidores hay agregados.
int RedServidores::cantidadServidores() const {
    return cantidad;
}

// Devuelve la cabeza de la lista de un servidor, o nullptr si no tiene.
const Arista* RedServidores::primeraArista(int id) const {
    if (!idValido(id)) {
        return nullptr;
    }
    return adyacencia[id];
}

// Agrega la arista en ambos sentidos, o false si los datos no son válidos.
bool RedServidores::agregarConexion(int a, int b, int latenciaMs) {
    // Solo se aceptan ids de servidores ya agregados.
    if (!idValido(a) || !idValido(b)) {
        return false;
    }
    // Sin autoenlaces porque no aportan una ruta entre servidores distintos.
    if (a == b) {
        return false;
    }
    // La latencia debe ser positiva para que Dijkstra tenga pesos válidos.
    if (latenciaMs <= 0) {
        return false;
    }
    // No se duplican aristas para mantener una sola latencia por enlace.
    if (existeConexion(a, b)) {
        return false;
    }
    // Va en ambas listas porque el grafo es no dirigido.
    Arista* haciaB = new Arista{b, latenciaMs, adyacencia[a]};
    adyacencia[a] = haciaB;
    Arista* haciaA = new Arista{a, latenciaMs, adyacencia[b]};
    adyacencia[b] = haciaA;
    AuditLogger::registrar("Conexion agregada: " + std::to_string(a) + "-" + std::to_string(b) + " latencia=" + std::to_string(latenciaMs));
    return true;
}

// Quita la arista de ambas listas, o false si no existía.
bool RedServidores::eliminarConexion(int a, int b) {
    // Solo se aceptan ids de servidores ya agregados.
    if (!idValido(a) || !idValido(b)) {
        return false;
    }
    // Debe existir en un sentido para intentar borrarla en ambos.
    if (!existeConexion(a, b)) {
        return false;
    }
    eliminarUnSentido(a, b);
    eliminarUnSentido(b, a);
    AuditLogger::registrar("Conexion eliminada: " + std::to_string(a) + "-" + std::to_string(b));
    return true;
}


// Calcula la ruta más corta delegando en Dijkstra (Dijkstra O(V^2)).
int RedServidores::rutaMasCorta(int origen, int destino) {
    // Solo se aceptan ids de servidores ya agregados.
    if (!idValido(origen) || !idValido(destino)) {
        ImpresorRutas::imprimirIdsInvalidos();
        AuditLogger::registrar("Ruta no calculada: ids invalidos origen=" + std::to_string(origen) + " destino=" + std::to_string(destino));
        return -1;
    }
    // El algoritmo devuelve el resultado y aquí solo se presenta y registra.
    Dijkstra algoritmo;
    ResultadoRuta resultado;
    algoritmo.calcular(*this, origen, destino, resultado);
    // El costo hacia uno mismo es cero aunque no haya saltos que mostrar.
    if (origen == destino) {
        AuditLogger::registrar("Ruta calculada: origen=" + std::to_string(origen) + " destino=" + std::to_string(destino) + " costo=0");
        return 0;
    }
    if (!resultado.existe()) {
        ImpresorRutas::imprimirSinRuta();
        AuditLogger::registrar("Ruta no encontrada: origen=" + std::to_string(origen) + " destino=" + std::to_string(destino));
        return -1;
    }
    ImpresorRutas::imprimirRuta(*this, resultado);
    AuditLogger::registrar("Ruta calculada: origen=" + std::to_string(origen) + " destino=" + std::to_string(destino) + " costo=" + std::to_string(resultado.costo()));
    return resultado.costo();
}

// Revisa con BFS si la red es conexa desde el servidor 0.
bool RedServidores::pingGeneral() {
    // Sin servidores no hay red que diagnosticar.
    if (cantidad <= 0) {
        ImpresorRutas::imprimirRedVacia();
        AuditLogger::registrar("Ping general: red vacia");
        return false;
    }
    // El recorrido devuelve el arreglo y esta clase es dueña de liberarlo.
    bool* visitado = BusquedaAnchura::recorrer(*this, 0);
    if (visitado == nullptr) {
        ImpresorRutas::imprimirRedVacia();
        AuditLogger::registrar("Ping general: red vacia");
        return false;
    }
    // Se cuentan los visitados para saber si todo fue alcanzado.
    int total = cantidad;
    int alcanzados = 0;
    for (int i = 0; i < total; ++i) {
        if (visitado[i]) {
            ++alcanzados;
        }
    }
    bool conexa = (alcanzados == total);
    if (conexa) {
        ImpresorRutas::imprimirRedConexa();
        AuditLogger::registrar("Ping general: red conexa");
    } else {
        ImpresorRutas::imprimirNoAlcanzados(*this, visitado, total);
        AuditLogger::registrar("Ping general: red no conexa");
    }
    // Se libera el arreglo del recorrido en ambas salidas.
    delete[] visitado;
    return conexa;
}

// Dice si el id está dentro del rango de servidores agregados.
bool RedServidores::idValido(int id) const {
    return id >= 0 && id < cantidad;
}

// Dice si ya existe la conexión entre a y b.
bool RedServidores::existeConexion(int a, int b) const {
    return latenciaEntre(a, b) != -1;
}

// Busca la latencia del enlace origen -> destino, o -1 si no existe.
int RedServidores::latenciaEntre(int a, int b) const {
    if (!idValido(a) || !idValido(b)) {
        return -1;
    }
    Arista* actual = adyacencia[a];
    while (actual != nullptr) {
        if (actual->destino == b) {
            return actual->latenciaMs;
        }
        actual = actual->siguiente;
    }
    return -1;
}

// Borra el enlace origen -> destino de una sola lista.
bool RedServidores::eliminarUnSentido(int origen, int destino) {
    if (!idValido(origen) || !idValido(destino)) {
        return false;
    }
    Arista* actual = adyacencia[origen];
    Arista* anterior = nullptr;
    // Se busca el nodo guardando el anterior para poder reenlazar al borrar.
    while (actual != nullptr && actual->destino != destino) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual == nullptr) {
        return false;
    }
    // Si es la cabeza se mueve el puntero del arreglo, si no se salta el nodo.
    if (anterior == nullptr) {
        adyacencia[origen] = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }
    delete actual;
    return true;
}
