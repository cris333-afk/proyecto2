/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Auditoría
 * Archivo     : auditoria.cpp
 * Descripción : Implementación de la escritura/lectura del log de
 *               auditoría en data/network_audit_log.txt (pendiente).
 * Autor       : (completar)
 * ========================================================================== */

#include "auditoria.hpp"

#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;

void AuditLogger::registrar(const std::string& detalle) {
    // El modo append conserva el historial de auditorías anterior.
    ofstream archivo("network_audit_log.txt", ios::app);
    if (!archivo.is_open()) {
        return;
    }

    const time_t momento = time(nullptr);
    char fechaHora[20] = {};
    if (strftime(fechaHora, sizeof(fechaHora), "%Y-%m-%d %H:%M:%S",
                 localtime(&momento)) == 0) {
        return;
    }

    archivo << "[" << fechaHora << "] " << detalle << '\n';
}

void AuditLogger::leerYMostrar() {
    ifstream archivo("network_audit_log.txt");
    if (!archivo.is_open()) {
        cout << "Aún no hay registros de auditoría.\n";
        return;
    }

    string linea;
    // Cada registro se imprime por separado para conservar su formato.
    while (getline(archivo, linea)) {
        cout << linea << '\n';
    }
}
