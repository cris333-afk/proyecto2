/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Punto de entrada
 * Archivo     : main.cpp
 * Descripción : Inicializa los servidores, carga los CSV y libera la memoria.
 * ========================================================================== */

#include <fstream>
#include <iostream>
#include <string>

#include "Servidor.h"
#include "auditoria/auditoria.hpp"
#include "menu/menu.hpp"
#include "estructuras/redservidores/RedServidores.h"

using namespace std;

namespace {

const int MAX_SERVIDORES = 20;

bool esEspacio(char caracter) {
    return caracter == ' ' || caracter == '\t' || caracter == '\r' || caracter == '\n';
}

string limpiar(const string& texto) {
    size_t inicio = 0;
    while (inicio < texto.size() && esEspacio(texto[inicio])) {
        ++inicio;
    }

    size_t fin = texto.size();
    while (fin > inicio && esEspacio(texto[fin - 1])) {
        --fin;
    }

    return texto.substr(inicio, fin - inicio);
}

bool esLineaIgnorable(const string& linea) {
    const string limpia = limpiar(linea);
    return limpia.empty() || limpia[0] == '#';
}

// Separa un campo sin usar contenedores de la STL.
bool obtenerCampo(const string& linea, int indice, char separador, string& resultado) {
    int campoActual = 0;
    size_t inicio = 0;

    for (size_t i = 0; i <= linea.size(); ++i) {
        if (i == linea.size() || linea[i] == separador) {
            if (campoActual == indice) {
                resultado = limpiar(linea.substr(inicio, i - inicio));
                return true;
            }
            ++campoActual;
            inicio = i + 1;
        }
    }

    return false;
}

bool convertirEntero(const string& texto, int& resultado) {
    const string limpio = limpiar(texto);
    if (limpio.empty()) {
        return false;
    }

    size_t inicio = 0;
    int signo = 1;
    if (limpio[0] == '-' || limpio[0] == '+') {
        signo = (limpio[0] == '-') ? -1 : 1;
        inicio = 1;
    }
    if (inicio == limpio.size()) {
        return false;
    }

    int valor = 0;
    for (size_t i = inicio; i < limpio.size(); ++i) {
        if (limpio[i] < '0' || limpio[i] > '9') {
            return false;
        }
        valor = valor * 10 + (limpio[i] - '0');
    }

    resultado = valor * signo;
    return true;
}

void cargarServidores(ContextoSistema& contexto) {
    ifstream archivo("data/servidores.csv");
    if (!archivo.is_open()) {
        cout << "No se encontro data/servidores.csv; se continuara sin carga inicial.\n";
        return;
    }

    string linea;
    int cargados = 0;
    while (getline(archivo, linea)) {
        if (esLineaIgnorable(linea)) {
            continue;
        }

        const string nombre = limpiar(linea);
        if (nombre.empty()) {
            continue;
        }

        const int id = contexto.red->agregarServidor(nombre);
        if (id < 0 || id >= contexto.maxServidores) {
            cout << "Servidor descartado por capacidad o nombre invalido: " << nombre << "\n";
            continue;
        }

        contexto.servidores[id].nombre = nombre;
        ++cargados;
    }

    if (cargados > 0) {
        contexto.servidorActual = 0;
    }
    AuditLogger::registrar("Carga inicial: servidores=" + to_string(cargados));
}

void cargarConexiones(ContextoSistema& contexto) {
    ifstream archivo("data/conexiones.csv");
    if (!archivo.is_open()) {
        cout << "No se encontro data/conexiones.csv; se continuara sin conexiones.\n";
        return;
    }

    string linea;
    int cargadas = 0;
    int descartadas = 0;
    while (getline(archivo, linea)) {
        if (esLineaIgnorable(linea)) {
            continue;
        }

        string origenTexto;
        string destinoTexto;
        string latenciaTexto;
        int origen = 0;
        int destino = 0;
        int latencia = 0;
        if (!obtenerCampo(linea, 0, ',', origenTexto) ||
            !obtenerCampo(linea, 1, ',', destinoTexto) ||
            !obtenerCampo(linea, 2, ',', latenciaTexto) ||
            !convertirEntero(origenTexto, origen) ||
            !convertirEntero(destinoTexto, destino) ||
            !convertirEntero(latenciaTexto, latencia)) {
            ++descartadas;
            continue;
        }

        if (contexto.red->agregarConexion(origen, destino, latencia)) {
            ++cargadas;
        } else {
            ++descartadas;
        }
    }

    AuditLogger::registrar("Carga inicial: conexiones=" + to_string(cargadas) +
                           " descartadas=" + to_string(descartadas));
}

void cargarRutas(ContextoSistema& contexto) {
    ifstream archivo("data/rutas.csv");
    if (!archivo.is_open()) {
        cout << "No se encontro data/rutas.csv; se continuara sin rutas.\n";
        return;
    }

    string linea;
    int cargadas = 0;
    int descartadas = 0;
    while (getline(archivo, linea)) {
        if (esLineaIgnorable(linea)) {
            continue;
        }

        string idTexto;
        string rutaPadre;
        string nombre;
        string tipoTexto;
        int id = 0;
        int tipo = 0;
        if (!obtenerCampo(linea, 0, ',', idTexto) ||
            !obtenerCampo(linea, 1, ',', rutaPadre) ||
            !obtenerCampo(linea, 2, ',', nombre) ||
            !obtenerCampo(linea, 3, ',', tipoTexto) ||
            !convertirEntero(idTexto, id) ||
            !convertirEntero(tipoTexto, tipo)) {
            ++descartadas;
            continue;
        }

        if (!contexto.red->idValido(id)) {
            ++descartadas;
            continue;
        }
        if (rutaPadre.empty()) {
            rutaPadre = "/";
        }

        if (contexto.servidores[id].archivos.crear(rutaPadre, nombre, tipo == 1)) {
            ++cargadas;
        } else {
            ++descartadas;
        }
    }

    AuditLogger::registrar("Carga inicial: rutas=" + to_string(cargadas) +
                           " descartadas=" + to_string(descartadas));
}

void cargarDatosIniciales(ContextoSistema& contexto) {
    // El orden importa: las rutas y conexiones referencian ids de servidores.
    cargarServidores(contexto);
    cargarConexiones(contexto);
    cargarRutas(contexto);
}

void liberarContexto(ContextoSistema& contexto) {
    delete contexto.red;
    contexto.red = nullptr;

    delete[] contexto.servidores;
    contexto.servidores = nullptr;
}

} // namespace

int main() {
    ContextoSistema contexto;
    contexto.maxServidores = MAX_SERVIDORES;
    contexto.servidores = new Servidor[MAX_SERVIDORES];
    contexto.servidorActual = -1;
    contexto.red = new RedServidores(MAX_SERVIDORES);

    AuditLogger::registrar("Network OS iniciado");
    cargarDatosIniciales(contexto);
    ejecutar_menu(contexto);
    AuditLogger::registrar("Network OS finalizado");

    liberarContexto(contexto);
    return 0;
}
