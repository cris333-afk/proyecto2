/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Árbol de directorios
 * Archivo     : arbol_directorios.hpp
 * Descripción : Declaraciones del árbol n-ario que modela el sistema de
 *               archivos: creación de nodos, búsqueda recursiva y
 *               eliminación en cascada. (Esqueleto: sin declaraciones aún.)
 * Autor       : (completar)
 * ========================================================================== */

#ifndef PROYECTO_ARBOL_DIRECTORIOS_HPP
#define PROYECTO_ARBOL_DIRECTORIOS_HPP

#include <string>

using namespace std;

struct NodoArchivo {
    std::string nombre;
    bool esCarpeta;
    NodoArchivo* primerHijo;
    NodoArchivo* siguienteHermano;
    NodoArchivo* padre;
};

class SistemaArchivos {
public:
    SistemaArchivos();
    ~SistemaArchivos();
    bool crear(const std::string& rutaPadre, const std::string& nombre, bool esCarpeta);
    NodoArchivo* buscar(const std::string& nombre) const;
    bool eliminar(const std::string& ruta);
    void mostrar() const;
    int contarNodos() const;

private:
    NodoArchivo* raiz;
};

#endif // PROYECTO_ARBOL_DIRECTORIOS_HPP
