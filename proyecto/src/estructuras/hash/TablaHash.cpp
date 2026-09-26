/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos (EIF207)
 * Módulo      : Tabla hash y autenticación
 * Archivo     : TablaHash.cpp
 * Descripción : Implementación de la tabla hash propia: función de dispersión
 *               polinomial (regla de Horner con primo 31), colisiones por
 *               encadenamiento simple y validación de usuarios en O(1)
 *               promedio. Manejo de memoria con new/new[] y delete/delete[].
 * Autor       : (completar)
 * ========================================================================== */

#include "TablaHash.h"

#include <iostream>

/* --------------------------------------------------------------------------
 * Integración con el módulo de auditoría (responsable: Cris).
 * De la auditoría solo se necesita el método estático
 * AuditLogger::registrar(detalle). Si AuditLogger.h ya está en la rama se usa
 * el real; si todavía no llegó, el respaldo permite compilar y probar ESTE
 * módulo de forma aislada sin tocar el contrato de la interfaz.
 * -------------------------------------------------------------------------- */
#if __has_include("AuditLogger.h")
#  include "AuditLogger.h"
#else
struct AuditLogger {
    static void registrar(const std::string& detalle) { (void)detalle; }
};
#endif

/* --------------------------------------------------------------------------
 * Constructor: reserva el arreglo dinámico de punteros y deja TODOS los
 * buckets en nullptr (tabla vacía). Sin este nullptr, cualquier recorrido de
 * cadena leería memoria basura.
 * -------------------------------------------------------------------------- */
TablaHash::TablaHash(int capacidad) {
    // Defensa: una capacidad <= 0 rompería el módulo (%) en obtenerIndice.
    if (capacidad <= 0) {
        capacidad = 101;
    }
    this->capacidad = capacidad;
    this->cantidad = 0;
    this->buckets = new NodoHash*[this->capacidad];
    for (int i = 0; i < this->capacidad; ++i) {
        this->buckets[i] = nullptr;
    }
}

/* --------------------------------------------------------------------------
 * Función de dispersión (hash polinomial) con la regla de Horner:
 *   h = (h * 31 + c) % capacidad
 * Se usa 31 porque es primo e impar: al multiplicar mezcla los bits ya
 * procesados con el carácter nuevo, evitando que anagramas ("ana"/"aan")
 * caigan en el mismo índice. El módulo se aplica EN CADA iteración para que
 * h nunca crezca sin control (evita el desbordamiento de unsigned long long).
 * -------------------------------------------------------------------------- */
int TablaHash::obtenerIndice(const std::string& usuario) const {
    unsigned long long h = 0;
    for (char c : usuario) {
        // (unsigned char) evita que un char negativo extienda signo y genere un
        // valor gigantesco al convertirlo a unsigned long long.
        h = (h * 31 + (unsigned char)c) % capacidad;
    }
    return (int)h;
}

/* --------------------------------------------------------------------------
 * Insertar: O(1) promedio.
 *  1. Dispersa el usuario para ubicar su bucket.
 *  2. Recorre SOLO esa cadena para rechazar duplicados (las llaves son únicas).
 *  3. Si no existe, crea el nodo y lo engancha al INICIO de la lista: no hay
 *     que recorrer nada, por eso la inserción es O(1) constante.
 * -------------------------------------------------------------------------- */
bool TablaHash::insertar(const std::string& usuario, const std::string& clave) {
    int indice = obtenerIndice(usuario);

    // Paso 2: detección de duplicados dentro del bucket correspondiente.
    for (NodoHash* actual = buckets[indice]; actual != nullptr; actual = actual->siguiente) {
        if (actual->usuario == usuario) {
            return false;  // ya registrado: no se sobrescribe la clave original
        }
    }

    // Paso 3: inserción al frente de la cadena (el "siguiente" es la cabeza previa).
    NodoHash* nuevo = new NodoHash;
    nuevo->usuario = usuario;
    nuevo->clave = clave;
    nuevo->siguiente = buckets[indice];
    buckets[indice] = nuevo;
    ++cantidad;

    AuditLogger::registrar("Usuario registrado: " + usuario);
    return true;
}

/* --------------------------------------------------------------------------
 * Existe: dispersa el usuario y busca ÚNICAMENTE dentro de la cadena de su
 * bucket (no recorre la tabla completa). Costo O(1 + longitud de la cadena).
 * -------------------------------------------------------------------------- */
bool TablaHash::existe(const std::string& usuario) const {
    int indice = obtenerIndice(usuario);
    for (NodoHash* actual = buckets[indice]; actual != nullptr; actual = actual->siguiente) {
        if (actual->usuario == usuario) {
            return true;
        }
    }
    return false;
}

/* --------------------------------------------------------------------------
 * Autenticar: mismo recorrido que existe(), pero además compara la clave.
 * Las llaves son únicas, así que al encontrar el usuario basta comparar su
 * clave: si no coincide es un fallo y se corta la búsqueda.
 * El evento queda registrado con el logger del sistema (éxito o fallo).
 * -------------------------------------------------------------------------- */
bool TablaHash::autenticar(const std::string& usuario, const std::string& clave) const {
    int indice = obtenerIndice(usuario);

    for (NodoHash* actual = buckets[indice]; actual != nullptr; actual = actual->siguiente) {
        if (actual->usuario == usuario) {
            if (actual->clave == clave) {
                AuditLogger::registrar("LOGIN EXITOSO: " + usuario);
                return true;
            }
            break;  // usuario hallado pero clave incorrecta => login fallido
        }
    }

    AuditLogger::registrar("LOGIN FALLIDO: " + usuario);
    return false;
}

/* --------------------------------------------------------------------------
 * Eliminar: cubre los 3 casos del borrado en una lista simplemente enlazada.
 * Se conserva el puntero "anterior" mientras se avanza, de modo que antes de
 * soltar un nodo con delete se reconecta el predecesor con el sucesor.
 *   1) Primer nodo  : no hay anterior  -> la cabeza pasa a ser actual->siguiente.
 *   2) Intermedio   : anterior->siguiente = actual->siguiente (se salta el nodo).
 *   3) Último nodo  : actual->siguiente es nullptr, así que la cadena termina
 *                     correctamente en el predecesor.
 * En los tres casos se libera el nodo y se decrementa el contador.
 * -------------------------------------------------------------------------- */
bool TablaHash::eliminar(const std::string& usuario) {
    int indice = obtenerIndice(usuario);

    NodoHash* actual = buckets[indice];
    NodoHash* anterior = nullptr;

    // Búsqueda del nodo objetivo manteniendo siempre al predecesor.
    while (actual != nullptr && actual->usuario != usuario) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == nullptr) {
        return false;  // el usuario no está en la tabla (bucket vacío o cadena agotada)
    }

    if (anterior == nullptr) {
        // Caso 1: el objetivo es el primer nodo de la cadena.
        buckets[indice] = actual->siguiente;
    } else {
        // Casos 2 y 3: el predecesor se enlaza al sucesor (que puede ser nullptr).
        // Este orden es clave: primero reconectar y solo después liberar.
        anterior->siguiente = actual->siguiente;
    }

    delete actual;  // libera el nodo exacto que se sacó de la cadena
    --cantidad;
    return true;
}

/* --------------------------------------------------------------------------
 * Destructor: libera TODA la memoria del objeto.
 * Por cada bucket se recorren los nodos de la cadena, pero el puntero al
 * siguiente se guarda ANTES del delete: si se liberara primero el nodo actual
 * se perdería el enlace y quedarían nodos huérfanos (fuga de memoria).
 * Al final se libera el arreglo dinámico de punteros con delete[] (siempre
 * delete[] porque se reservó con new[]).
 * -------------------------------------------------------------------------- */
TablaHash::~TablaHash() {
    for (int i = 0; i < capacidad; ++i) {
        NodoHash* actual = buckets[i];
        while (actual != nullptr) {
            NodoHash* siguiente = actual->siguiente;  // se guarda el enlace ...
            delete actual;                            // ... y recién ahí se libera
            actual = siguiente;
        }
        buckets[i] = nullptr;
    }
    delete[] buckets;
    buckets = nullptr;
}

/* --------------------------------------------------------------------------
 * mostrarEstadisticas: reporte de calidad de la dispersión.
 *   - factor de carga = cantidad / capacidad (cuánto se está llenando la tabla)
 *   - cadena de colisiones más larga: si crece mucho, la dispersión es pobre
 *     y las búsquedas se degradan de O(1) a O(n) en el peor caso.
 * -------------------------------------------------------------------------- */
void TablaHash::mostrarEstadisticas() const {
    double factorCarga = (capacidad > 0)
                             ? static_cast<double>(cantidad) / static_cast<double>(capacidad)
                             : 0.0;

    int cadenaMasLarga = 0;
    int bucketsOcupados = 0;

    for (int i = 0; i < capacidad; ++i) {
        int longitud = 0;
        for (NodoHash* actual = buckets[i]; actual != nullptr; actual = actual->siguiente) {
            ++longitud;
        }
        if (longitud > 0) {
            ++bucketsOcupados;
        }
        if (longitud > cadenaMasLarga) {
            cadenaMasLarga = longitud;
        }
    }

    std::cout << "--- Estadisticas de la Tabla Hash ---" << std::endl;
    std::cout << "Capacidad (buckets)        : " << capacidad << std::endl;
    std::cout << "Usuarios almacenados       : " << cantidad << std::endl;
    std::cout << "Factor de carga            : " << factorCarga
              << "  (cantidad / capacidad)" << std::endl;
    std::cout << "Buckets ocupados           : " << bucketsOcupados
              << " de " << capacidad << std::endl;
    std::cout << "Cadena de colisiones mayor : " << cadenaMasLarga << std::endl;
    std::cout << "------------------------------------" << std::endl;
}
