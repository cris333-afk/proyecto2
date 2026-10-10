/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Menú interactivo
 * Archivo     : menu.cpp
 * Descripción : Integra el menú con un Servidor por id de la red.
 * ========================================================================== */

#include "menu.hpp"

#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "../auditoria/auditoria.hpp"

using namespace std;

static string leerLinea(const string& prompt) {
    cout << prompt;
    string texto;
    getline(cin, texto);
    return texto;
}

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

static bool servidorValido(const ContextoSistema& contexto, int id) {
    return contexto.servidores != nullptr && contexto.red != nullptr &&
           id >= 0 && id < contexto.maxServidores && contexto.red->idValido(id);
}

static Servidor* servidorActivo(ContextoSistema& contexto) {
    if (!servidorValido(contexto, contexto.servidorActual)) {
        return nullptr;
    }
    return &contexto.servidores[contexto.servidorActual];
}

static void mostrarServidorActivo(const ContextoSistema& contexto) {
    cout << "\nServidor activo: ";
    if (!servidorValido(contexto, contexto.servidorActual)) {
        cout << "ninguno\n";
        return;
    }

    cout << contexto.servidorActual << " - "
         << contexto.servidores[contexto.servidorActual].nombre << "\n";
}

void mostrar_menu() {
    cout << "\n===== Network OS =====\n";
    cout << "ARCHIVOS (servidor activo)\n";
    cout << setw(3) << 1 << ". Crear\n";
    cout << setw(3) << 2 << ". Buscar\n";
    cout << setw(3) << 3 << ". Mostrar\n";
    cout << setw(3) << 4 << ". Eliminar cascada\n";
    cout << "\nUSUARIOS (servidor activo)\n";
    cout << setw(3) << 5 << ". Registrar\n";
    cout << setw(3) << 6 << ". Autenticar\n";
    cout << setw(3) << 7 << ". Estadisticas\n";
    cout << "\nRED (Dijkstra + BFS)\n";
    cout << setw(3) << 8 << ". Agregar servidor\n";
    cout << setw(3) << 9 << ". Agregar conexion\n";
    cout << setw(3) << 10 << ". Eliminar conexion\n";
    cout << setw(3) << 11 << ". Ruta mas corta\n";
    cout << setw(3) << 12 << ". Ping general\n";
    cout << "\nCONTROL\n";
    cout << setw(3) << 13 << ". Leer bitacora\n";
    cout << setw(3) << 14 << ". Seleccionar servidor\n";
    cout << setw(3) << 0 << ". Salir\n";
}

static void opCrear(ContextoSistema& contexto) {
    Servidor* servidor = servidorActivo(contexto);
    if (servidor == nullptr) {
        cout << "No hay servidor activo.\n";
        return;
    }

    const string padre = leerLinea("Ruta padre (ej. /): ");
    const string nombre = leerLinea("Nombre: ");
    const string tipo = leerLinea("Tipo (1=carpeta, 0=archivo): ");
    const bool ok = servidor->archivos.crear(
        padre.empty() ? "/" : padre, nombre, tipo == "1");
    cout << (ok ? "Creado.\n" : "No creado (padre, nombre o duplicado).\n");
    AuditLogger::registrar(string("ARCHIVO crear ") + nombre);
}

static void opRegistrar(ContextoSistema& contexto) {
    Servidor* servidor = servidorActivo(contexto);
    if (servidor == nullptr) {
        cout << "No hay servidor activo.\n";
        return;
    }

    const string usuario = leerLinea("Usuario: ");
    const string clave = leerLinea("Clave: ");
    cout << "Indice: " << servidor->usuarios.obtenerIndice(usuario) << "\n";
    cout << (servidor->usuarios.insertar(usuario, clave) ? "Registrado.\n" : "Duplicado.\n");
}

static void opAutenticar(ContextoSistema& contexto) {
    Servidor* servidor = servidorActivo(contexto);
    if (servidor == nullptr) {
        cout << "No hay servidor activo.\n";
        return;
    }

    const string usuario = leerLinea("Usuario: ");
    const string clave = leerLinea("Clave: ");
    cout << (servidor->usuarios.autenticar(usuario, clave) ? "LOGIN OK.\n" : "LOGIN FALLO.\n");
}

void ejecutar_menu(ContextoSistema& contexto) {
    int opcion = -1;
    while (opcion != 0) {
        mostrarServidorActivo(contexto);
        mostrar_menu();
        if (!leerEntero("Opcion: ", opcion)) {
            cout << "Opcion invalida.\n";
            continue;
        }

        Servidor* servidor = servidorActivo(contexto);
        if (opcion == 1) {
            opCrear(contexto);
        } else if (opcion == 2) {
            if (servidor == nullptr) {
                cout << "No hay servidor activo.\n";
            } else {
                const string nombre = leerLinea("Nombre: ");
                cout << (servidor->archivos.buscar(nombre) != nullptr
                             ? "Encontrado.\n"
                             : "No encontrado.\n");
            }
        } else if (opcion == 3) {
            if (servidor == nullptr) {
                cout << "No hay servidor activo.\n";
            } else {
                servidor->archivos.mostrar();
            }
        } else if (opcion == 4) {
            if (servidor == nullptr) {
                cout << "No hay servidor activo.\n";
            } else {
                const string ruta = leerLinea("Ruta (ej. /docs): ");
                cout << (servidor->archivos.eliminar(ruta) ? "Eliminado.\n" : "No eliminado.\n");
                AuditLogger::registrar(string("ARCHIVO eliminar ") + ruta);
            }
        } else if (opcion == 5) {
            opRegistrar(contexto);
        } else if (opcion == 6) {
            opAutenticar(contexto);
        } else if (opcion == 7) {
            if (servidor == nullptr) {
                cout << "No hay servidor activo.\n";
            } else {
                servidor->usuarios.mostrarEstadisticas();
            }
        } else if (opcion == 8) {
            const string nombre = leerLinea("Servidor: ");
            const int id = contexto.red->agregarServidor(nombre);
            if (id >= 0 && id < contexto.maxServidores) {
                contexto.servidores[id].nombre = nombre;
                contexto.servidorActual = id;
                cout << "Servidor creado con id=" << id << "\n";
            } else {
                cout << "No se pudo crear el servidor.\n";
            }
        } else if (opcion == 9) {
            int origen = 0;
            int destino = 0;
            int latencia = 0;
            if (!leerEntero("Origen: ", origen) || !leerEntero("Destino: ", destino) ||
                !leerEntero("Latencia: ", latencia)) {
                cout << "Parametros invalidos.\n";
                continue;
            }
            cout << (contexto.red->agregarConexion(origen, destino, latencia)
                         ? "Agregada.\n"
                         : "No agregada.\n");
        } else if (opcion == 10) {
            int origen = 0;
            int destino = 0;
            if (!leerEntero("Origen: ", origen) || !leerEntero("Destino: ", destino)) {
                cout << "Parametros invalidos.\n";
                continue;
            }
            cout << (contexto.red->eliminarConexion(origen, destino)
                         ? "Eliminada.\n"
                         : "No existia.\n");
        } else if (opcion == 11) {
            int origen = 0;
            int destino = 0;
            if (!leerEntero("Origen: ", origen) || !leerEntero("Destino: ", destino)) {
                cout << "Parametros invalidos.\n";
                continue;
            }
            contexto.red->rutaMasCorta(origen, destino);
        } else if (opcion == 12) {
            contexto.red->pingGeneral();
        } else if (opcion == 13) {
            AuditLogger::leerYMostrar();
        } else if (opcion == 14) {
            int id = 0;
            if (!leerEntero("Id del servidor: ", id) || !servidorValido(contexto, id)) {
                cout << "Servidor invalido.\n";
            } else {
                contexto.servidorActual = id;
                cout << "Servidor seleccionado: " << id << "\n";
            }
        } else if (opcion == 0) {
            cout << "Saliendo.\n";
        } else {
            cout << "Opcion invalida.\n";
        }
    }
}
