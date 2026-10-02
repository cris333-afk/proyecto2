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

// Árbol de directorios de un servidor. Por ahora define la raíz "/", la
// creación de nodos y un auxiliar privado para resolver rutas. Las demás
// operaciones (buscar, eliminar, mostrar, contarNodos y el destructor) se
// agregarán después.
class SistemaArchivos {
public:
    SistemaArchivos();  // crea la raíz "/" como carpeta

    // Crea un archivo o carpeta como hijo directo del nodo rutaPadre.
    // Devuelve false si rutaPadre no existe, no es carpeta o si ya hay
    // un hijo con ese mismo nombre dentro de ese padre.
    bool crear(const std::string& rutaPadre,
               const std::string& nombre,
               bool esCarpeta);

private:
    NodoArchivo* raiz;  // raíz del sistema de archivos ("/")

    // Resuelve una ruta absoluta ("/", "/docs", "/docs/txt") desde la raíz y
    // devuelve el nodo correspondiente, o nullptr si algún nivel no existe.
    // Recorre la ruta completa, por lo que no depende de buscar(nombre).
    NodoArchivo* navegar(const std::string& ruta) const;
};

#endif // PROYECTO_ARBOL_DIRECTORIOS_HPP
