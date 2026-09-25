/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Auditoría
 * Archivo     : auditoria.hpp
 * Descripción : Declaraciones del registro de auditoría: escritura y
 *               lectura de data/network_audit_log.txt (una línea por
 *               operación del sistema). (Esqueleto: sin declaraciones aún.)
 * Autor       : (completar)
 * ========================================================================== */

#ifndef PROYECTO_AUDITORIA_HPP
#define PROYECTO_AUDITORIA_HPP

#include <string>

using namespace std;

class AuditLogger {
public:
    static void registrar(const std::string& detalle);
    static void leerYMostrar();
};

#endif // PROYECTO_AUDITORIA_HPP
