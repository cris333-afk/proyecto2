/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Punto de entrada
 * Archivo     : main.cpp
 * Descripción : Inicializa el sistema (árbol, hash y red), lanza el menú
 *               interactivo y libera toda la memoria al salir.
 * ========================================================================== */

#include <iostream>

#include "menu/menu.hpp"
#include "auditoria/auditoria.hpp"
#include "estructuras/arbol/arbol_directorios.hpp"
#include "estructuras/hash/tabla_hash.hpp"
#include "estructuras/redservidores/RedServidores.h"

using namespace std;

int main() {
    // Se reserva en heap porque el curso exige new/delete explícitos.
    ContextoSistema ctx;
    ctx.archivos = new SistemaArchivos();
    ctx.usuarios = new TablaHash(101);
    ctx.red = new RedServidores(20);

    AuditLogger::registrar("Network OS iniciado");
    ejecutar_menu(ctx);
    AuditLogger::registrar("Network OS finalizado");

    // Se libera en orden inverso para no dejar new sin delete.
    delete ctx.red;
    delete ctx.usuarios;
    delete ctx.archivos;
    ctx.red = nullptr;
    ctx.usuarios = nullptr;
    ctx.archivos = nullptr;
    return 0;
}
