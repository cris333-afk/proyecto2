/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Tabla hash
 * Archivo     : tabla_hash.hpp
 * Descripción : Declaraciones de la tabla hash propia, con función de
 *               dispersión manual y manejo de colisiones. (Esqueleto.)
 * Autor       : (completar)
 * ========================================================================== */

#ifndef PROYECTO_TABLA_HASH_HPP
#define PROYECTO_TABLA_HASH_HPP

#include <string>

using namespace std;

struct NodoHash {
    std::string usuario;
    std::string clave;
    NodoHash* siguiente;
};

class TablaHash {
public:
    TablaHash(int capacidad = 101);
    ~TablaHash();
    bool insertar(const std::string& usuario, const std::string& clave);
    bool existe(const std::string& usuario) const;
    bool autenticar(const std::string& usuario, const std::string& clave) const;
    bool eliminar(const std::string& usuario);
    int obtenerIndice(const std::string& usuario) const;
    void mostrarEstadisticas() const;

private:
    int capacidad;
    NodoHash** buckets;
};

#endif // PROYECTO_TABLA_HASH_HPP
