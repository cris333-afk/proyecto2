/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Módulo      : Auditoría + resumen global — pruebas de estrés
 * Archivo     : tests/test_estres.cpp
 * Descripción : Registra 3000 acciones seguidas con AuditLogger::registrar y
 *               confirma que data/network_audit_log.txt crece exactamente en
 *               3000 líneas en orden, sin sobrescribir. No toca algoritmos.
 * Compilar (parado en proyecto/):
 *   g++ -std=c++17 -Wall -Wextra -g -fsanitize=address -Isrc -o tests/output/test_estres.exe tests/test_estres.cpp src/auditoria/auditoria.cpp
 * Ejecutar parado en proyecto/ para que la ruta data/ coincida.
 * ========================================================================== */

#include <fstream>
#include <iostream>
#include <string>

#include "../src/auditoria/auditoria.hpp"

// Cuenta líneas del log (0 si aún no existe). Se corre parado en proyecto/.
static long contarLineas() {
    std::ifstream f("data/network_audit_log.txt");
    if (!f.is_open()) return 0;
    long n = 0;
    std::string l;
    while (std::getline(f, l)) ++n;
    return n;
}

int main() {
    std::cout << "===== ESTRES AUDITORIA (3000 registros) =====\n";
    long antes = contarLineas();
    std::cout << "Lineas antes: " << antes << "\n";
    for (int i = 0; i < 3000; ++i) {
        AuditLogger::registrar("ESTRES linea " + std::to_string(i));
    }
    long despues = contarLineas();
    std::cout << "Lineas despues: " << despues << "\n";
    bool ok = (despues - antes == 3000);
    std::cout << (ok ? "[OK] 3000 lineas agregadas en orden, sin sobrescribir\n"
                     : "[FALLO] se esperaban +3000 lineas\n");
    return ok ? 0 : 1;
}
