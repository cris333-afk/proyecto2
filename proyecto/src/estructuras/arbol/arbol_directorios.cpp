/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Árbol de directorios
 * Archivo     : arbol_directorios.cpp
 * Descripción : Implementación del árbol n-ario de directorios: raíz "/" y
 *               creación de archivos/carpetas por ruta completa.
 * Autor       : (completar)
 * ========================================================================== */

#include "arbol_directorios.hpp"

// Crea el nodo raíz del sistema de archivos: la carpeta "/".
// Se reserva con new y queda sin hijos, sin hermano y sin padre.
SistemaArchivos::SistemaArchivos() {
    raiz = new NodoArchivo;
    raiz->nombre = "/";
    raiz->esCarpeta = true;
    raiz->primerHijo = nullptr;
    raiz->siguienteHermano = nullptr;
    raiz->padre = nullptr;
}

// Resuelve una ruta absoluta recorriéndola desde la raíz "/".
// Se separa la ruta por '/' y en cada nivel se avanza por la lista de
// hermanos hasta encontrar el hijo con ese nombre. Si un nivel no existe,
// se devuelve nullptr. No usa buscar(nombre) para evitar ambigüedades de
// nombres repetidos en carpetas distintas.
NodoArchivo* SistemaArchivos::navegar(const std::string& ruta) const {
    NodoArchivo* actual = raiz;
    std::string segmento;

    for (size_t i = 0; i <= ruta.size(); ++i) {
        // El '/' (y el final de la cadena) cierran el segmento actual.
        if (i == ruta.size() || ruta[i] == '/') {
            if (!segmento.empty()) {
                NodoArchivo* hijo = actual->primerHijo;
                while (hijo != nullptr && hijo->nombre != segmento) {
                    hijo = hijo->siguienteHermano;
                }
                if (hijo == nullptr) {
                    return nullptr;  // el segmento no existe en este nivel
                }
                actual = hijo;
                segmento.clear();
            }
        } else {
            segmento += ruta[i];
        }
    }
    return actual;
}

// Crea un archivo o carpeta como hijo directo del nodo indicado por rutaPadre.
bool SistemaArchivos::crear(const std::string& rutaPadre,
                            const std::string& nombre,
                            bool esCarpeta) {
    // 1) Resolver la ruta completa del padre desde la raíz.
    NodoArchivo* padre = navegar(rutaPadre);
    if (padre == nullptr) {
        return false;  // la ruta del padre no existe
    }

    // 2) El padre debe ser una carpeta, no un archivo.
    if (!padre->esCarpeta) {
        return false;
    }

    // 3) Rechazar si ya existe un hijo con el mismo nombre en este padre.
    NodoArchivo* hijo = padre->primerHijo;
    while (hijo != nullptr) {
        if (hijo->nombre == nombre) {
            return false;  // nombre duplicado dentro del mismo padre
        }
        hijo = hijo->siguienteHermano;
    }

    // 4) Reservar e inicializar el nuevo nodo.
    NodoArchivo* nuevo = new NodoArchivo;
    nuevo->nombre = nombre;
    nuevo->esCarpeta = esCarpeta;
    nuevo->primerHijo = nullptr;
    nuevo->siguienteHermano = nullptr;
    nuevo->padre = padre;

    // 5) Insertar al final de la lista de hijos (primerHijo / siguienteHermano).
    if (padre->primerHijo == nullptr) {
        padre->primerHijo = nuevo;  // primer hijo del padre
    } else {
        NodoArchivo* ultimo = padre->primerHijo;
        while (ultimo->siguienteHermano != nullptr) {
            ultimo = ultimo->siguienteHermano;
        }
        ultimo->siguienteHermano = nuevo;  // enlazar como nuevo hermano
    }

    return true;
}
