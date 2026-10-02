/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Árbol de directorios
 * Archivo     : arbol_directorios.cpp
 * Descripción : Implementación del árbol n-ario de directorios. Por ahora
 *               solo el constructor que reserva la raíz "/" del sistema.
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
