/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Árbol de directorios
 * Archivo     : arbol_directorios.hpp
 * Descripción : Declaraciones del árbol n-ario que modela el sistema de
 *               archivos: creación de nodos, búsqueda recursiva y
 *               eliminación en cascada. (Estructura base: nodo y clase.)
 * Autor       : (completar)
 * ========================================================================== */

#ifndef PROYECTO_ARBOL_DIRECTORIOS_HPP
#define PROYECTO_ARBOL_DIRECTORIOS_HPP

#include <string>

// Nodo del árbol n-ario. Cada carpeta o archivo guarda:
//   - primerHijo       : primer elemento de la lista de hijos.
//   - siguienteHermano : siguiente hijo del mismo padre.
//   - padre            : nodo contenedor (nullptr en la raíz).
// Esta representación permite una cantidad ilimitada de hijos sin usar STL.
struct NodoArchivo {
    std::string nombre;
    bool esCarpeta;
    NodoArchivo* primerHijo;
    NodoArchivo* siguienteHermano;
    NodoArchivo* padre;
};

// Árbol de directorios de un servidor. Por ahora solo define la estructura
// base: el constructor que crea la raíz "/". Las operaciones (crear, buscar,
// eliminar, mostrar, contarNodos y el destructor) se agregarán después.
class SistemaArchivos {
public:
    SistemaArchivos();  // crea la raíz "/" como carpeta

private:
    NodoArchivo* raiz;  // raíz del sistema de archivos ("/")
};

#endif // PROYECTO_ARBOL_DIRECTORIOS_HPP
