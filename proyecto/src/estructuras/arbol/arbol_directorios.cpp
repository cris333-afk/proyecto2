/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Árbol de directorios
 * Archivo     : arbol_directorios.cpp
 * Descripción : Implementación del árbol n-ario de directorios: raíz "/" y
 *               creación de archivos/carpetas por ruta completa.
 * Autor       : (Liseth Briones)
 * ========================================================================== */

#include "arbol_directorios.hpp"

#include <iostream>
#include <string>

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

// Libera TODO el árbol al destruir el objeto, reutilizando la eliminación en
// cascada existente sobre la raíz. A diferencia de eliminar(), aquí sí se
// libera la raíz, porque el objeto SistemaArchivos completo deja de existir.
SistemaArchivos::~SistemaArchivos() {
    eliminarSubarbol(raiz);
    raiz = nullptr;  // evita dejar el puntero a memoria liberada
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

    //Validacion de nombre vacio
    if (nombre.empty()) {
        return false;
    }

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

// Busca recursivamente un nodo por nombre comenzando desde la raíz.
NodoArchivo* SistemaArchivos::buscar(const std::string& nombre) const {

    //Validacion de nombre, muestra null si el nombre esta vacio
    if (nombre.empty()) {
    return nullptr;
}

    return buscarPrivada(raiz, nombre);
}

// Auxiliar recursivo de buscar(): recorre en profundidad el árbol revisando
// el nodo actual, su subárbol (primerHijo) y los hermanos de cada nivel
// (siguienteHermano). Devuelve la primera coincidencia o nullptr.
NodoArchivo* SistemaArchivos::buscarPrivada(NodoArchivo* nodo,
                                            const std::string& nombre) const {
    if (nodo == nullptr) {
        return nullptr;  // fin de una rama o de la lista de hermanos
    }
    if (nodo->nombre == nombre) {
        return nodo;  // coincidencia en el nodo actual
    }
    // Primero buscar en el subárbol del primer hijo...
    NodoArchivo* encontrado = buscarPrivada(nodo->primerHijo, nombre);
    if (encontrado != nullptr) {
        return encontrado;
    }
    // ...y si no aparece, continuar con el siguiente hermano.
    return buscarPrivada(nodo->siguienteHermano, nombre);
}

// Elimina un archivo o carpeta (con todo su contenido) mediante su ruta
// completa. No usa buscar(nombre) para localizar el nodo: resuelve la ruta
// desde la raíz con navegar(), evitando ambigüedades por nombres repetidos.
bool SistemaArchivos::eliminar(const std::string& ruta) {
    // 1) Resolver la ruta completa desde la raíz.
    NodoArchivo* nodo = navegar(ruta);
    if (nodo == nullptr) {
        return false;  // la ruta no existe
    }

    // 2) No se permite eliminar la raíz "/".
    if (nodo == raiz) {
        return false;
    }

    // 3) Desvincular el nodo de la lista de hijos de su padre.
    NodoArchivo* padre = nodo->padre;
    if (padre->primerHijo == nodo) {
        // Es el primer hijo: el padre pasa a apuntar al siguiente hermano.
        padre->primerHijo = nodo->siguienteHermano;
    } else {
        // Es un hijo intermedio o el último: buscar el hermano anterior.
        NodoArchivo* anterior = padre->primerHijo;
        while (anterior != nullptr && anterior->siguienteHermano != nodo) {
            anterior = anterior->siguienteHermano;
        }
        if (anterior != nullptr) {
            anterior->siguienteHermano = nodo->siguienteHermano;
        }
    }

    // 4) Liberar el nodo y todo su subárbol (postorden).
    eliminarSubarbol(nodo);

    return true;
}

// Libera el nodo y todos sus descendientes en postorden: primero recorre y
// elimina los hijos y, al final, borra el propio nodo. Se guarda el
// siguienteHermano ANTES de eliminar el hijo para no acceder a memoria ya
// liberada.
void SistemaArchivos::eliminarSubarbol(NodoArchivo* nodo) {
    if (nodo == nullptr) {
        return;  // nada que liberar
    }

    NodoArchivo* hijo = nodo->primerHijo;
    while (hijo != nullptr) {
        NodoArchivo* siguiente = hijo->siguienteHermano;  // guardar ANTES
        eliminarSubarbol(hijo);                           // recursión a las hojas
        hijo = siguiente;
    }

    delete nodo;  // se libera al final (postorden)
}

// Muestra todo el árbol desde la raíz con nivel inicial 0.
void SistemaArchivos::mostrar() const {
    mostrarRecursivo(raiz, 0);
}

// Imprime el nodo con sangría de 2 * nivel espacios y recorre su subárbol.
// Las carpetas llevan "/" al final; la raíz ya se llama "/", por lo que no
// se le agrega otro para no imprimir "//".
void SistemaArchivos::mostrarRecursivo(NodoArchivo* nodo, int nivel) const {
    if (nodo == nullptr) {
        return;  // fin de una rama o de la lista de hermanos
    }

    
    // 1) Sangría de 2 espacios por cada nivel de profundidad.
    std::cout << std::string(2 * nivel, ' ');

    // 2) Nombre del nodo; las carpetas se muestran con "/" al final.
    std::cout << nodo->nombre;
    if (nodo->esCarpeta && nodo->nombre != "/") {
        std::cout << "/";
    }
    std::cout << "\n";

    // 3) Recorrer los hijos (un nivel más abajo) y los hermanos (mismo nivel).
    mostrarRecursivo(nodo->primerHijo, nivel + 1);
    mostrarRecursivo(nodo->siguienteHermano, nivel);
}

// Cuenta todos los nodos del árbol, incluida la raíz "/".
int SistemaArchivos::contarNodos() const {
    return contarRecursivo(raiz);
}

// Auxiliar recursivo: suma 1 por el nodo actual y delega en sus hijos
// (primerHijo) y hermanos (siguienteHermano), respetando la representación
// del árbol sin modificarlo.
int SistemaArchivos::contarRecursivo(NodoArchivo* nodo) const {
    if (nodo == nullptr) {
        return 0;  // no hay nada que contar
    }
    int total = 1;  // el nodo actual
    total += contarRecursivo(nodo->primerHijo);
    total += contarRecursivo(nodo->siguienteHermano);
    return total;
}
