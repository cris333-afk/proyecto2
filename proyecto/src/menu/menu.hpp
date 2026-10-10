/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Menú interactivo
 * Archivo     : menu.hpp
 * Descripción : Declaraciones del menú y del contexto de servidores.
 * ========================================================================== */

#ifndef PROYECTO_MENU_HPP
#define PROYECTO_MENU_HPP

#include <string>

#include "../Servidor.h"
#include "../estructuras/redservidores/RedServidores.h"

using namespace std;

// El índice del arreglo coincide con el id que entrega RedServidores.
struct ContextoSistema {
    Servidor* servidores;
    int maxServidores;
    int servidorActual;
    RedServidores* red;
};

void mostrar_menu();

// Bucle de lectura de opción y llamado a los módulos.
void ejecutar_menu(ContextoSistema& contexto);

#endif // PROYECTO_MENU_HPP
