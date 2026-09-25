/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Grafo ponderado
 * Archivo     : grafo.hpp
 * Descripción : Declaraciones del grafo ponderado que modela la red del
 *               Network OS (nodos = dispositivos, aristas = enlaces con
 *               costo). (Esqueleto: sin declaraciones aún.)
 * Autor       : (completar)
 * ========================================================================== */

#ifndef PROYECTO_GRAFO_HPP
#define PROYECTO_GRAFO_HPP

#include <string>

using namespace std;

struct Arista {
    int destino;
    int latenciaMs;
    Arista* siguiente;
};

class RedServidores {
public:
    RedServidores(int maxServidores = 20);
    ~RedServidores();
    int agregarServidor(const std::string& nombre);
    bool agregarConexion(int a, int b, int latenciaMs);
    bool eliminarConexion(int a, int b);
    int rutaMasCorta(int origen, int destino);
    bool pingGeneral();
    std::string nombreDe(int id) const;

private:
    int maxServidores;
    std::string* nombres;
    Arista** adyacencia;
};

#endif // PROYECTO_GRAFO_HPP
