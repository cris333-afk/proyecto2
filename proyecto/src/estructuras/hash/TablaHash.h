/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos (EIF207)
 * Módulo      : Tabla hash y autenticación
 * Archivo     : TablaHash.h
 * Descripción : Declaración de la tabla hash construida 100% desde cero (sin
 *               contenedores de la STL) que valida usuarios en O(1) promedio.
 *               Resuelve colisiones por ENCADENAMIENTO (lista simple por
 *               bucket) y dispersa con la regla de Horner y el primo 31.
 * Autor       : (completar)
 * ========================================================================== */

#ifndef TABLA_HASH_H
#define TABLA_HASH_H

#include <string>

// Nodo de la lista simplemente enlazada que cuelga de cada bucket.
struct NodoHash {
    std::string usuario;   // llave de búsqueda
    std::string clave;     // credencial asociada
    NodoHash* siguiente;   // enlace al siguiente nodo de la misma cadena
};

class TablaHash {
public:
    TablaHash(int capacidad = 101);  // 101 primo: reparte mejor y reduce colisiones
    ~TablaHash();                    // libera cada nodo y el arreglo de punteros

    bool insertar(const std::string& usuario, const std::string& clave);
    bool existe(const std::string& usuario) const;
    bool autenticar(const std::string& usuario, const std::string& clave) const;
    bool eliminar(const std::string& usuario);
    int  obtenerIndice(const std::string& usuario) const;
    void mostrarEstadisticas() const;

private:
    NodoHash** buckets;  // arreglo dinámico: cada casilla es la cabeza de una lista
    int capacidad;       // número de buckets de la tabla
    int cantidad;        // usuarios almacenados actualmente
};

#endif
