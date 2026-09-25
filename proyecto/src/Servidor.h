#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <string>

#include "estructuras/arbol/arbol_directorios.hpp"
#include "estructuras/hash/tabla_hash.hpp"

using namespace std;

struct Servidor {
    std::string nombre;
    SistemaArchivos archivos;
    TablaHash usuarios;
};

#endif
