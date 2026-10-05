// TEMPORAL: se reemplaza por el de Cris
#include "AuditLogger.h"

#include <iostream>

// Guarda el detalle (temporal: se imprime porque aún no existe el log real).
void AuditLogger::registrar(const std::string& detalle) {
    std::cout << "[AUDIT] " << detalle << std::endl;
}
