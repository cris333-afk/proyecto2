/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Menú interactivo
 * Archivo     : menu.hpp
 * Descripción : Declaraciones del menú interactivo de consola del
 *               Network OS: muestra opciones y deriva a cada módulo.
 * ========================================================================== */

#ifndef PROYECTO_MENU_HPP
#define PROYECTO_MENU_HPP

#include <string>

#include "../estructuras/arbol/arbol_directorios.hpp"
#include "../estructuras/hash/tabla_hash.hpp"
#include "../estructuras/redservidores/RedServidores.h"

using namespace std;

// Contexto vivo del sistema: lo crea main y lo usa el menú sin copiarlo.
struct ContextoSistema {
    SistemaArchivos* archivos;
    TablaHash* usuarios;
    RedServidores* red;
};

// Muestra las opciones disponibles del Network OS.
void mostrar_menu();

// Bucle de lectura de opción y llamado a los módulos.
// Recibe el contexto ya inicializado (árbol, hash y red).
void ejecutar_menu(ContextoSistema& ctx);

#endif // PROYECTO_MENU_HPP
