#include "RedServidores.h"
#include "AuditLogger.h"

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

// Calcula la ruta más corta con Dijkstra O(V^2).
// No admite pesos negativos porque asume que el nodo extraído ya es final;
// su complejidad es O(V^2) por elegir el mínimo con un recorrido lineal.
int RedServidores::rutaMasCorta(int origen, int destino) {
    // Paso 1: solo se aceptan ids de servidores ya agregados.
    if (!idValido(origen) || !idValido(destino)) {
        std::cout << "Ruta no calculada: ids de servidor invalidos." << std::endl;
        AuditLogger::registrar("Ruta no calculada: ids invalidos origen=" + std::to_string(origen) + " destino=" + std::to_string(destino));
        return -1;
    }
    // El costo hacia uno mismo es cero y no requiere recorrer la red.
    if (origen == destino) {
        AuditLogger::registrar("Ruta calculada: origen=" + std::to_string(origen) + " destino=" + std::to_string(destino) + " costo=0");
        return 0;
    }
    // Paso 2: se reserva el estado de Dijkstra con new[] por la regla del curso.
    const int INF = 1000000000;
    int total = cantidad;
    int* costos = new int[total];
    int* predecesor = new int[total];
    bool* visitado = new bool[total];
    for (int i = 0; i < total; ++i) {
        costos[i] = INF;
        predecesor[i] = -1;
        visitado[i] = false;
    }
    costos[origen] = 0;
    // Paso 3: se fija un nodo por vuelta hasta visitar todos los alcanzables.
    for (int vuelta = 0; vuelta < total; ++vuelta) {
        // Se elige con recorrido lineal el no visitado de menor costo.
        int u = -1;
        int menor = INF;
        for (int i = 0; i < total; ++i) {
            if (!visitado[i] && costos[i] < menor) {
                menor = costos[i];
                u = i;
            }
        }
        // Si el menor sigue en INF ya no quedan nodos alcanzables.
        if (u == -1) {
            break;
        }
        visitado[u] = true;
        // Paso 4: se relajan las aristas que salen del nodo recién fijado.
        Arista* arista = adyacencia[u];
        while (arista != nullptr) {
            int v = arista->destino;
            int w = arista->latenciaMs;
            // Solo se relaja con costo conocido para evitar desbordamiento.
            if (costos[u] != INF && costos[u] + w < costos[v]) {
                costos[v] = costos[u] + w;
                predecesor[v] = u;
            }
            arista = arista->siguiente;
        }
    }
    // Paso 5: si el destino quedó en INF es porque no hay camino.
    if (costos[destino] == INF) {
        std::cout << "No existe ruta entre los servidores indicados." << std::endl;
        AuditLogger::registrar("Ruta no encontrada: origen=" + std::to_string(origen) + " destino=" + std::to_string(destino));
        // Se liberan los arreglos en esta salida de error para no dejar fugas.
        delete[] costos;
        delete[] predecesor;
        delete[] visitado;
        return -1;
    }
    int costoTotal = costos[destino];
    // Paso 6: se reconstruye la ruta desde el destino hasta el origen.
    int* ruta = new int[total];
    int largo = 0;
    int actual = destino;
    // Se sigue el predecesor porque cada nodo guarda por dónde se llegó.
    while (actual != -1) {
        ruta[largo] = actual;
        ++largo;
        actual = predecesor[actual];
    }
    // Paso 7: se imprime la ruta al revés con nombres y latencias exactas.
    std::cout << "Ruta:";
    // Se recorre al revés porque el arreglo quedó de destino a origen.
    for (int i = largo - 1; i >= 0; --i) {
        std::cout << " " << nombres[ruta[i]];
        if (i > 0) {
            std::cout << " ->";
        }
    }
    std::cout << std::endl;
    for (int i = largo - 1; i > 0; --i) {
        // La latencia se lee de la lista porque la ruta solo guarda ids.
        int latencia = latenciaEntre(ruta[i], ruta[i - 1]);
        std::cout << "  " << nombres[ruta[i]] << " -> " << nombres[ruta[i - 1]] << " : " << latencia << " ms" << std::endl;
    }
    std::cout << "Costo total: " << costoTotal << " ms" << std::endl;
    AuditLogger::registrar("Ruta calculada: origen=" + std::to_string(origen) + " destino=" + std::to_string(destino) + " costo=" + std::to_string(costoTotal));
    // Paso 8: se liberan todos los arreglos temporales en la salida normal.
    delete[] ruta;
    delete[] costos;
    delete[] predecesor;
    delete[] visitado;
    // Paso 10: se devuelve el costo mínimo encontrado.
    return costoTotal;
}

// Revisa con BFS si la red es conexa desde el servidor 0.
// Se usa BFS y no DFS porque recorre por niveles, ideal para el ping.
bool RedServidores::pingGeneral() {
    // Paso 1: sin servidores no hay red que diagnosticar.
    if (cantidad <= 0) {
        std::cout << "Ping general: no hay servidores en la red." << std::endl;
        AuditLogger::registrar("Ping general: red vacia");
        return false;
    }
    // Paso 2: cola circular propia sobre un arreglo de tamaño n.
    // Es circular para reutilizar el espacio con módulo y no desplazar elementos.
    int total = cantidad;
    int* cola = new int[total];
    int frente = 0;
    int final = 0;
    int elementos = 0;
    // Paso 3: BFS desde el servidor 0 con visitado en false.
    bool* visitado = new bool[total];
    for (int i = 0; i < total; ++i) {
        visitado[i] = false;
    }
    // Se encola el 0 y se marca para no visitarlo dos veces.
    cola[final] = 0;
    final = (final + 1) % total;
    ++elementos;
    visitado[0] = true;
    while (elementos > 0) {
        // Se saca el frente avanzando con módulo por ser circular.
        int actual = cola[frente];
        frente = (frente + 1) % total;
        --elementos;
        Arista* arista = adyacencia[actual];
        while (arista != nullptr) {
            int vecino = arista->destino;
            // Solo se encolan vecinos nuevos para no repetir visitas.
            if (!visitado[vecino]) {
                visitado[vecino] = true;
                cola[final] = vecino;
                final = (final + 1) % total;
                ++elementos;
            }
            arista = arista->siguiente;
        }
    }
    // Paso 4: se cuentan los visitados para saber si todo fue alcanzado.
    int alcanzados = 0;
    for (int i = 0; i < total; ++i) {
        if (visitado[i]) {
            ++alcanzados;
        }
    }
    bool conexa = (alcanzados == total);
    if (conexa) {
        std::cout << "Red conexa: todos los servidores son alcanzables." << std::endl;
        AuditLogger::registrar("Ping general: red conexa");
    } else {
        // Paso 5: se listan los no alcanzados con su diagnóstico.
        std::cout << "Servidores no alcanzados:" << std::endl;
        for (int i = 0; i < total; ++i) {
            if (!visitado[i]) {
                // Cabeza en nullptr significa que nunca tuvo conexiones.
                if (adyacencia[i] == nullptr) {
                    std::cout << "  " << nombres[i] << " (id " << i << "): AISLADO" << std::endl;
                } else {
                    std::cout << "  " << nombres[i] << " (id " << i << "): RED PARTIDA" << std::endl;
                }
            }
        }
        AuditLogger::registrar("Ping general: red no conexa");
    }
    // Paso 6: se liberan los arreglos temporales en ambas salidas.
    delete[] cola;
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
