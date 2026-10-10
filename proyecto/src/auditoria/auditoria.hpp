/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Auditoría
 * Archivo     : auditoria.hpp
 * Descripción : Declaraciones del registro de auditoría: escritura y
 *               lectura de network_audit_log.txt (una línea por
 *               operación del sistema).
 * Autor       : Cris
 * ========================================================================== */

#ifndef PROYECTO_AUDITORIA_HPP
#define PROYECTO_AUDITORIA_HPP

#include <string>

using namespace std;

// Ruta única del log de auditoría (versionada fuera: ver .gitignore).
static const char* const RUTA_LOG_AUDITORIA = "network_audit_log.txt";

class AuditLogger {
public:
    static void registrar(const std::string& detalle);
    static void leerYMostrar();
    // Alias pedido en el esqueleto original: lee y muestra el log.
    static void leer_log();
};

#endif // PROYECTO_AUDITORIA_HPP
