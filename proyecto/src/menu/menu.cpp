/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Menú interactivo
 * Archivo     : menu.cpp
 * Descripción : Implementación del menú interactivo de consola: deriva a
 *               arbol_directorios, tabla_hash, RedServidores y auditoría.
 * ========================================================================== */

#include "menu.hpp"

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

#include "../auditoria/auditoria.hpp"

using namespace std;

// Lee una línea completa tras limpiar el buffer numérico.
static string leerLinea(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

// Lee un entero validando el fallo de cin.
static bool leerEntero(const string& prompt, int& salida) {
    cout << prompt;
    if (!(cin >> salida)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

void mostrar_menu() {
    cout << "\n===== Network OS =====\n";
    cout << "ARCHIVOS (arbol)\n";
    cout << setw(3) << 1 << ". Crear\n";
    cout << setw(3) << 2 << ". Buscar\n";
    cout << setw(3) << 3 << ". Mostrar\n";
    cout << setw(3) << 4 << ". Eliminar cascada\n";
    cout << "\nUSUARIOS (hash)\n";
    cout << setw(3) << 5 << ". Registrar\n";
    cout << setw(3) << 6 << ". Autenticar\n";
    cout << setw(3) << 7 << ". Estadisticas\n";
    cout << "\nRED (Dijkstra + BFS)\n";
    cout << setw(3) << 8 << ". Agregar servidor\n";
    cout << setw(3) << 9 << ". Agregar conexion\n";
    cout << setw(3) << 10 << ". Eliminar conexion\n";
    cout << setw(3) << 11 << ". Ruta mas corta\n";
    cout << setw(3) << 12 << ". Ping general\n";
    cout << "\nAUDITORIA\n";
    cout << setw(3) << 13 << ". Leer log\n";
    cout << setw(3) << 0 << ". Salir\n";
}



static void opCrear(ContextoSistema& ctx) {
    string padre = leerLinea("Ruta padre (ej. /): ");
    string nombre = leerLinea("Nombre: ");
    string tipo = leerLinea("Tipo (1=carpeta, 0=archivo): ");
    bool ok = ctx.archivos->crear(padre.empty() ? "/" : padre, nombre, tipo == "1");
    cout << (ok ? "Creado.\n" : "No creado (padre/dup).\n");
    AuditLogger::registrar(string("ARCHIVO crear ") + nombre);
}

static void opRegistrar(ContextoSistema& ctx) {
    string u = leerLinea("Usuario: ");
    string c = leerLinea("Clave: ");
    cout << "Indice: " << ctx.usuarios->obtenerIndice(u) << "\n";
    cout << (ctx.usuarios->insertar(u, c) ? "Registrado.\n" : "Duplicado.\n");
}

static void opAutenticar(ContextoSistema& ctx) {
    string u = leerLinea("Usuario: ");
    string c = leerLinea("Clave: ");
    cout << (ctx.usuarios->autenticar(u, c) ? "LOGIN OK.\n" : "LOGIN FALLO.\n");
}

void ejecutar_menu(ContextoSistema& ctx) {
    int op = -1;
    while (op != 0) {
        mostrar_menu();
        if (!leerEntero("Opcion: ", op)) {
            cout << "Opcion invalida.\n";
            continue;
        }
        if (op == 1) opCrear(ctx);
        else if (op == 2) {
            string n = leerLinea("Nombre: ");
            cout << (ctx.archivos->buscar(n) != nullptr ? "Encontrado.\n" : "No encontrado.\n");
        }
        else if (op == 3) ctx.archivos->mostrar();
        else if (op == 4) {
            string r = leerLinea("Ruta (ej. /docs): ");
            cout << (ctx.archivos->eliminar(r) ? "Eliminado.\n" : "No eliminado.\n");
            AuditLogger::registrar(string("ARCHIVO eliminar ") + r);
        }
        else if (op == 5) opRegistrar(ctx);
        else if (op == 6) opAutenticar(ctx);
        else if (op == 7) ctx.usuarios->mostrarEstadisticas();
        else if (op == 8) {
            string n = leerLinea("Servidor: ");
            int id = ctx.red->agregarServidor(n);
            cout << "id=" << id << "\n";
        }
        else if (op == 9) {
            int a, b, lat;
            if (!leerEntero("Origen: ", a) || !leerEntero("Destino: ", b) ||
                !leerEntero("Latencia: ", lat)) { cout << "Invalidos.\n"; continue; }
            cout << (ctx.red->agregarConexion(a, b, lat) ? "Agregada.\n" : "No agregada.\n");
        }
        else if (op == 10) {
            int a, b;
            if (!leerEntero("Origen: ", a) || !leerEntero("Destino: ", b)) { cout << "Invalidos.\n"; continue; }
            cout << (ctx.red->eliminarConexion(a, b) ? "Eliminada.\n" : "No existia.\n");
        }
        else if (op == 11) {
            int a, b;
            if (!leerEntero("Origen: ", a) || !leerEntero("Destino: ", b)) { cout << "Invalidos.\n"; continue; }
            ctx.red->rutaMasCorta(a, b);
        }
        else if (op == 12) ctx.red->pingGeneral();
        else if (op == 13) AuditLogger::leer_log();
        else if (op == 0) cout << "Saliendo.\n";
        else cout << "Opcion invalida.\n";
    }
}
