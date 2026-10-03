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

// Árbol de directorios de un servidor. Define la raíz "/", la creación de
// nodos, la búsqueda recursiva por nombre, la eliminación en cascada por
// ruta y un auxiliar privado para resolver rutas. Las demás operaciones
// (mostrar, contarNodos y el destructor) se agregarán después.
class SistemaArchivos {
public:
    SistemaArchivos();  // crea la raíz "/" como carpeta

    // Crea un archivo o carpeta como hijo directo del nodo rutaPadre.
    // Devuelve false si rutaPadre no existe, no es carpeta o si ya hay
    // un hijo con ese mismo nombre dentro de ese padre.
    bool crear(const std::string& rutaPadre,
               const std::string& nombre,
               bool esCarpeta);

    // Busca recursivamente el primer nodo cuyo nombre coincida con el
    // recibido y devuelve su puntero; nullptr si no existe en el árbol.
    // Es una búsqueda por nombre, no por ruta: si el mismo nombre aparece
    // en carpetas distintas, devuelve la coincidencia según el recorrido.
    NodoArchivo* buscar(const std::string& nombre) const;

    // Elimina un archivo o carpeta mediante su ruta completa. Si es una
    // carpeta, elimina también todo su contenido (en cascada). Devuelve
    // false si la ruta no existe o si se intenta eliminar la raíz "/".
    bool eliminar(const std::string& ruta);

private:
    NodoArchivo* raiz;  // raíz del sistema de archivos ("/")

    // Resuelve una ruta absoluta ("/", "/docs", "/docs/txt") desde la raíz y
    // devuelve el nodo correspondiente, o nullptr si algún nivel no existe.
    // Recorre la ruta completa, por lo que no depende de buscar(nombre).
    NodoArchivo* navegar(const std::string& ruta) const;

    // Auxiliar recursivo de buscar(): revisa el nodo actual, luego su
    // primerHijo y finalmente los hermanos mediante siguienteHermano.
    NodoArchivo* buscarPrivada(NodoArchivo* nodo,
                               const std::string& nombre) const;

    // Libera el nodo y todos sus descendientes en postorden (primero los
    // hijos y, al final, el propio nodo).
    void eliminarSubarbol(NodoArchivo* nodo);
};

#endif // PROYECTO_ARBOL_DIRECTORIOS_HPP
