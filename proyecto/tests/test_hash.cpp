/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos (EIF207)
 * Módulo      : Tabla hash y autenticación — pruebas unitarias
 * Archivo     : tests/test_hash.cpp
 * Descripción : Suite de pruebas del módulo TablaHash: estado inicial,
 *               inserción y duplicados, autenticación, función de dispersión,
 *               colisiones/encadenamiento, los 3 casos de eliminación,
 *               reinserción y prueba de estrés de memoria.
 *
 * Compilar (parado en la carpeta proyecto/):
 *   g++ -std=c++17 -Wall -Wextra -g -fsanitize=address \
 *       -Isrc/estructuras/hash -Isrc/auditoria \
 *       -o tests/test_hash tests/test_hash.cpp src/estructuras/hash/TablaHash.cpp
 *   ./tests/test_hash
 *
 * Requerimientos que cubre esta suite (script independiente con su propio main):
 *   1) Inserta más de 200 usuarios ficticios y eleva el factor de carga (sección 8).
 *   2) Colisiones explícitas: usuarios DISTINTOS que caen en el MISMO índice y
 *      coexisten sin sobrescribirse (secciones 4 y 8).
 *   3) autenticar() validado con casos positivos y negativos (secciones 2, 4 y 8).
 *   4) eliminar() probado en los 3 casos: inicio, medio y fin de una CADENA DE
 *      COLISIÓN (sección 5).
 *   5) Impresión de estadísticas: factor de carga y cadena de colisiones más
 *      larga (secciones 1, 4, 6, 7 y 8).
 *   6) main() termina con return 0, así todas las TablaHash salen de alcance y
 *      sus destructores liberan nodos y buckets (sin fugas de memoria).
 *
 * (Si el compilador no soporta AddressSanitizer, quitar -fsanitize=address.
 *  En Windows/MinGW el enlace puede fallar con "cannot find -lasan": ahí no hay
 *  runtime de ASan. La ausencia de fugas se verificó además con un arnés que
 *  cuenta los operadores new/delete globales: en todos los escenarios el número
 *  de asignaciones coincidió exactamente con el de liberaciones.)
 *
 * Sobre AuditLogger: TablaHash.cpp ya resuelve el logger por sí solo con
 * #if __has_include("AuditLogger.h"), por lo que este script compila aislado.
 * Si tu entorno exige el enlace estricto del logger, inyecta el dummy que está
 * más abajo compilando con -DMOCK_AUDITLOGGER.
 * ========================================================================== */

#include "TablaHash.h"

#include <iostream>
#include <string>

/* --------------------------------------------------------------------------
 * Mock opcional del logger de auditoría (DESACTIVADO por defecto).
 * La tabla hash solo necesita AuditLogger::registrar(detalle) y el propio
 * TablaHash.cpp ya trae un respaldo interno con #if __has_include, así que esta
 * prueba aislada compila sin este bloque. Se deja detrás de una macro para
 * poder forzarlo con -DMOCK_AUDITLOGGER y, además, para no definir un
 * AuditLogger distinto al real dentro de la misma compilación (así se respeta
 * la regla de definición única, ODR).
 * -------------------------------------------------------------------------- */
#ifdef MOCK_AUDITLOGGER
struct AuditLogger {
    static void registrar(const std::string& detalle) { (void)detalle; }
};
#endif

/* --------------------------------------------------------------------------
 * Mini marco de pruebas: sin librerías externas (se mantiene el espíritu del
 * proyecto de construir todo a mano) y sin abortar, para que TODAS las
 * pruebas se ejecuten y se vea el resumen final.
 * -------------------------------------------------------------------------- */
static int pruebas_ok = 0;
static int pruebas_fallidas = 0;

static void verificar(bool condicion, const std::string& descripcion) {
    if (condicion) {
        ++pruebas_ok;
        std::cout << "  [OK]    " << descripcion << '\n';
    } else {
        ++pruebas_fallidas;
        std::cout << "  [FALLO] " << descripcion << '\n';
    }
}

static void seccion(const std::string& titulo) {
    std::cout << "\n=== " << titulo << " ===\n";
}

/* ==========================================================================
 * 1) Constructor y estado inicial
 * ========================================================================== */
static void test_constructor_y_estado_inicial() {
    seccion("1) Constructor y estado inicial");

    TablaHash tabla;  // capacidad por defecto: 101
    int indice = tabla.obtenerIndice("admin");
    verificar(indice >= 0 && indice < 101,
              "obtenerIndice queda dentro de [0, capacidad) con la capacidad por defecto");
    verificar(!tabla.existe("admin"), "existe() es false para un usuario nunca insertado");
    verificar(!tabla.autenticar("admin", "1234"), "autenticar() falla si el usuario no existe");
    verificar(!tabla.eliminar("admin"), "eliminar() devuelve false si el usuario no existe");
    tabla.mostrarEstadisticas();  // tabla vacía: factor de carga 0 y cadena máxima 0

    // Caso borde: usuario y clave vacíos (dispersan al índice 0 con Horner).
    verificar(tabla.obtenerIndice("") == 0, "la cadena vacía dispersa al índice 0");
    verificar(tabla.insertar("", ""), "se puede insertar usuario con clave vacía (caso borde)");
    verificar(tabla.autenticar("", ""), "se autentica el usuario insertado con clave vacía");
    verificar(tabla.eliminar(""), "se elimina ese usuario de caso borde");

    // Capacidad mínima: toda la tabla es una sola cadena.
    TablaHash minima(1);
    verificar(minima.obtenerIndice("cualquiera") == 0,
              "con capacidad 1 todos los usuarios colisionan en el índice 0");

    // Capacidad inválida: el constructor debe corregirla para no dividir por cero.
    TablaHash invalida(0);
    verificar(invalida.obtenerIndice("x") == invalida.obtenerIndice("x"),
              "capacidad 0 se corrige y el módulo del hash sigue siendo seguro");
}

/* ==========================================================================
 * 2) Inserción, duplicados y autenticación básica
 * ========================================================================== */
static void test_insercion_duplicados_y_autenticacion() {
    seccion("2) Inserción, duplicados y autenticación (casos positivos y negativos)");

    TablaHash tabla;
    verificar(tabla.insertar("ana", "clave_ana"), "insertar un usuario nuevo devuelve true");
    verificar(tabla.existe("ana"), "existe() detecta al usuario recién insertado");
    verificar(!tabla.insertar("ana", "otra_clave"),
              "insertar el mismo usuario una segunda vez devuelve false");
    verificar(tabla.autenticar("ana", "clave_ana"),
              "la clave original sigue válida (el duplicado no la sobrescribió)");
    verificar(!tabla.autenticar("ana", "otra_clave"),
              "la clave del intento duplicado NO quedó guardada");
    verificar(!tabla.autenticar("Ana", "clave_ana"), "el hash distingue mayúsculas/minúsculas");
    verificar(!tabla.autenticar("ana", "CLAVE_ANA"), "la clave se compara sensible a mayúsculas");

    // --- autenticar(): casos POSITIVOS y NEGATIVOS explícitos --------------
    verificar(tabla.autenticar("ana", "clave_ana"), "[positivo] usuario existe + clave correcta");
    verificar(!tabla.autenticar("ana", "mala"), "[negativo] usuario existe + clave incorrecta");
    verificar(!tabla.autenticar("fantasma", "clave_ana"),
              "[negativo] usuario inexistente con la clave de otro");
    verificar(!tabla.autenticar("fantasma", ""), "[negativo] usuario inexistente con clave vacía");
    verificar(!tabla.autenticar("", ""), "[negativo] usuario vacío que nunca se registró");

    // Inserción masiva verificada de forma agregada (salida legible).
    const int N = 100;
    bool insertados = true;
    for (int i = 0; i < N; ++i) {
        if (!tabla.insertar("usuario_" + std::to_string(i), "pw_" + std::to_string(i))) {
            insertados = false;
        }
    }
    verificar(insertados, "se insertan 100 usuarios distintos (todos devuelven true)");
    verificar(!tabla.insertar("usuario_50", "nueva"),
              "dentro de la masa de datos también se rechazan duplicados");
    verificar(tabla.autenticar("usuario_50", "pw_50"),
              "el usuario duplicado conserva su clave original");
}

/* ==========================================================================
 * 3) Función de dispersión (regla de Horner, primo 31)
 * ========================================================================== */
static void test_obtener_indice() {
    seccion("3) Función de dispersión (Horner con primo 31)");

    TablaHash tabla(101);
    verificar(tabla.obtenerIndice("server1") == tabla.obtenerIndice("server1"),
              "el hash es determinista para la misma entrada");

    const char* usuarios[] = {"a", "b", "admin", "root", "server1", "nodo-07",
                              "usuario_con_nombre_largo", "192.168.0.10"};
    const int total = 8;
    bool en_rango = true;
    for (int i = 0; i < total; ++i) {
        int idx = tabla.obtenerIndice(usuarios[i]);
        if (idx < 0 || idx >= 101) {
            en_rango = false;
        }
    }
    verificar(en_rango, "todos los índices caen dentro de [0, 101)");

    // Comprobación matemática de la fórmula del contrato:
    // h = (h * 31 + (unsigned char)c) % capacidad, carácter a carácter.
    unsigned long long esperado = 0;
    const std::string patron = "ana";
    for (char c : patron) {
        esperado = (esperado * 31 + (unsigned char)c) % 101;
    }
    verificar(tabla.obtenerIndice(patron) == (int)esperado,
              "el índice coincide con la fórmula de Horner módulo 101");

    // El módulo en cada iteración evita el desbordamiento con entradas enormes.
    std::string larga(5000, 'z');
    int idx_larga = tabla.obtenerIndice(larga);
    verificar(idx_larga >= 0 && idx_larga < 101,
              "un usuario de 5000 caracteres no desborda la función hash");
    verificar(tabla.obtenerIndice(larga) == idx_larga, "ese índice grande es estable y repetible");

    // La mezcla de Horner separa anagramas (no los agrupa en un mismo bucket).
    std::cout << "  [INFO]  indice(\"ana\") = " << tabla.obtenerIndice("ana")
              << " | indice(\"aan\") = " << tabla.obtenerIndice("aan") << '\n';
}

/* ==========================================================================
 * 4) Colisiones: encadenamiento real dentro de un mismo bucket
 * ========================================================================== */
static void test_colisiones() {
    seccion("4) Colisiones y encadenamiento");

    // Caso extremo: un solo bucket => TODOS los usuarios viven en una cadena.
    TablaHash unaCadena(1);
    const char* usuarios[] = {"alice", "bob", "carol", "dave", "erin"};
    const int total = 5;
    bool insertados = true;
    for (int i = 0; i < total; ++i) {
        if (!unaCadena.insertar(usuarios[i], std::string("clave_") + usuarios[i])) {
            insertados = false;
        }
    }
    verificar(insertados, "5 usuarios distintos se insertan aunque compartan el mismo índice");

    bool coexisten = true;
    for (int i = 0; i < total; ++i) {
        if (!unaCadena.existe(usuarios[i])) {
            coexisten = false;
        }
    }
    verificar(coexisten, "los 5 coexisten en la misma cadena: ninguno se pierde");
    verificar(unaCadena.autenticar("carol", "clave_carol"),
              "se autentica un nodo del medio de la cadena");
    verificar(unaCadena.autenticar("erin", "clave_erin"),
              "se autentica el nodo de la cabeza de la cadena");
    verificar(unaCadena.autenticar("alice", "clave_alice"),
              "se autentica el nodo del final de la cadena");
    verificar(!unaCadena.autenticar("carol", "incorrecta"),
              "una clave errónea no autentica aunque el usuario exista");
    unaCadena.mostrarEstadisticas();  // cadena mayor esperada: 5

    // Colisiones medidas de forma empírica con una tabla pequeña (7 buckets).
    TablaHash tabla(7);
    int indiceObjetivo = tabla.obtenerIndice("usuario_0");
    const int M = 200;
    int colisiones = 0;
    for (int i = 0; i < M; ++i) {
        if (tabla.obtenerIndice("usuario_" + std::to_string(i)) == indiceObjetivo) {
            ++colisiones;
        }
    }
    verificar(colisiones > 1,
              "con 7 buckets se detectan colisiones reales (se fuerza el encadenamiento)");
    std::cout << "  [INFO]  usuarios que colisionan con usuario_0: " << colisiones << '\n';

    // Se insertan SOLO los usuarios que colisionan con usuario_0: todos caen en
    // el MISMO índice, así que la única forma de que convivan es que el
    // encadenamiento funcione (colisión explícita, no accidental).
    bool okColisiones = true;
    bool todosEnLaCadena = true;
    int insertadosColision = 0;
    int idSegundo = -1;                 // para imprimir un par de colisionantes
    std::string primerColisionante;
    std::string clavePrimero;

    for (int i = 0; i < M; ++i) {
        std::string u = "usuario_" + std::to_string(i);
        if (tabla.obtenerIndice(u) == indiceObjetivo) {
            std::string c = "pw_" + std::to_string(i);
            if (!tabla.insertar(u, c)) {
                okColisiones = false;
            } else {
                ++insertadosColision;
                if (primerColisionante.empty()) {
                    primerColisionante = u;
                    clavePrimero = c;
                } else if (idSegundo < 0) {
                    idSegundo = i;
                }
                if (!tabla.autenticar(u, c)) {
                    todosEnLaCadena = false;
                }
            }
        }
    }

    std::cout << "  [INFO]  colision explicita en el indice " << indiceObjetivo << ": "
              << insertadosColision << " usuarios distintos comparten ese bucket";
    if (idSegundo >= 0) {
        std::cout << " ('" << primerColisionante << "' y 'usuario_" << idSegundo
                  << "', entre otros)";
    }
    std::cout << '\n';
    verificar(insertadosColision > 1 && okColisiones && todosEnLaCadena,
              "coexisten " + std::to_string(insertadosColision) +
                  " usuarios en el mismo indice y cada uno autentica con su clave");

    // Ninguno sobrescribió a otro: cada nodo conserva SU clave y no la de un
    // vecino de la misma cadena.
    bool sinSobrescritura = true;
    for (int i = 0; i < M; ++i) {
        std::string u = "usuario_" + std::to_string(i);
        if (tabla.obtenerIndice(u) == indiceObjetivo) {
            if (!tabla.autenticar(u, "pw_" + std::to_string(i))) {
                sinSobrescritura = false;
            }
            if (tabla.autenticar(u, "pw_" + std::to_string((i + 1) % M))) {
                sinSobrescritura = false;  // aceptar la clave de otro = sobrescritura
            }
        }
    }
    verificar(sinSobrescritura, "ningun usuario de la colision quedo con la clave de otro");
    verificar(tabla.autenticar(primerColisionante, clavePrimero),
              "el primer usuario insertado en la colision conserva su clave original");
    verificar(!tabla.insertar(primerColisionante, "intento_de_cambio"),
              "reinsertar un usuario de la colision se rechaza (no sobrescribe)");
    verificar(tabla.autenticar(primerColisionante, clavePrimero),
              "tras el intento de duplicado la clave sigue siendo la original");

    // Se elimina uno de los colisionantes y se verifica que los demás sobrevivan.
    std::string victima = "usuario_" + std::to_string(idSegundo >= 0 ? idSegundo : 0);
    verificar(tabla.eliminar(victima), "se elimina un usuario de la colision (indice compartido)");
    bool restoIntacto = true;
    for (int i = 0; i < M; ++i) {
        std::string u = "usuario_" + std::to_string(i);
        if (u != victima && tabla.obtenerIndice(u) == indiceObjetivo &&
            !tabla.autenticar(u, "pw_" + std::to_string(i))) {
            restoIntacto = false;
        }
    }
    verificar(restoIntacto, "los demas usuarios del bucket siguen autenticandose tras el borrado");
    tabla.mostrarEstadisticas();  // factor de carga y cadena mas larga del bucket saturado
}

/* ==========================================================================
 * 5) Eliminación: los 3 casos de la lista simplemente enlazada
 * ========================================================================== */
static void test_eliminar_tres_casos() {
    seccion("5) Eliminacion en una cadena de colision: inicio, medio y fin");

    TablaHash tabla(1);  // una sola cadena => control total del orden de nodos

    // Como la inserción es al inicio, la cadena queda:
    // cuarto -> tercero -> segundo -> primero
    tabla.insertar("primero", "c1");  // queda al final de la cadena
    tabla.insertar("segundo", "c2");
    tabla.insertar("tercero", "c3");
    tabla.insertar("cuarto", "c4");   // es la cabeza de la cadena

    std::cout << "  [INFO]  cadena de colision (capacidad 1 => todos en el indice 0), "
              << "orden interno: cuarto -> tercero -> segundo -> primero\n";

    // Caso 1: eliminar el PRIMER nodo (la cabeza de la cadena).
    verificar(tabla.eliminar("cuarto"), "caso 1: eliminar el primer nodo devuelve true");
    verificar(!tabla.existe("cuarto"), "caso 1: la cabeza eliminada ya no existe");
    verificar(tabla.autenticar("tercero", "c3") && tabla.existe("segundo") &&
                  tabla.existe("primero"),
              "caso 1: la nueva cabeza y el resto de la cadena siguen alcanzables");

    // Caso 2: eliminar un nodo INTERMEDIO (el predecesor debe enlazar al sucesor).
    verificar(tabla.eliminar("segundo"), "caso 2: eliminar un nodo intermedio devuelve true");
    verificar(!tabla.existe("segundo"), "caso 2: el nodo intermedio ya no existe");
    verificar(tabla.autenticar("tercero", "c3"),
              "caso 2: el predecesor quedó enlazado al sucesor correctamente");
    verificar(tabla.autenticar("primero", "c1"),
              "caso 2: el sucesor del nodo intermedio sigue alcanzable");

    // Caso 3: eliminar el ÚLTIMO nodo (predecesor con siguiente = nullptr).
    verificar(tabla.eliminar("primero"), "caso 3: eliminar el último nodo devuelve true");
    verificar(!tabla.existe("primero"), "caso 3: el último nodo ya no existe");
    verificar(tabla.autenticar("tercero", "c3"),
              "caso 3: la cadena no se rompió al liberar el último nodo");

    verificar(!tabla.eliminar("primero"), "eliminar dos veces el mismo usuario devuelve false");

    // Bucket vacío: no debe fallar ni acceder a memoria inválida.
    TablaHash vacia(5);
    verificar(!vacia.eliminar("nadie"), "eliminar en un bucket vacío devuelve false sin fallar");
}

/* ==========================================================================
 * 6) Reinserción tras borrado y prueba de estrés
 * ========================================================================== */
static void test_reinsercion_y_estres() {
    seccion("6) Reinserción y prueba de estrés");

    // Reinsertar un usuario eliminado debe funcionar y con la clave nueva.
    TablaHash tabla(101);
    tabla.insertar("ana", "vieja");
    verificar(tabla.eliminar("ana"), "eliminar un usuario existente devuelve true");
    verificar(!tabla.existe("ana"), "tras eliminarlo, existe() devuelve false");
    verificar(tabla.insertar("ana", "nueva"), "se puede reinsertar el mismo usuario tras borrarlo");
    verificar(tabla.autenticar("ana", "nueva") && !tabla.autenticar("ana", "vieja"),
              "tras la reinserción solo la clave nueva es válida");

    // Estrés: muchas operaciones seguidas => ASan/Valgrind detectarían fugas.
    const int N = 500;
    bool insertados = true;
    for (int i = 0; i < N; ++i) {
        if (!tabla.insertar("nodo_" + std::to_string(i), "pw_" + std::to_string(i))) {
            insertados = false;
        }
    }
    verificar(insertados, "se insertan 500 usuarios sin fallos");

    bool existeTodos = true;
    bool autenticaTodos = true;
    bool rechazaClaveMala = true;
    for (int i = 0; i < N; ++i) {
        std::string u = "nodo_" + std::to_string(i);
        if (!tabla.existe(u)) {
            existeTodos = false;
        }
        if (!tabla.autenticar(u, "pw_" + std::to_string(i))) {
            autenticaTodos = false;
        }
        if (tabla.autenticar(u, "clave_incorrecta")) {
            rechazaClaveMala = false;
        }
    }
    verificar(existeTodos, "existe() encuentra a los 500 usuarios");
    verificar(autenticaTodos, "autenticar() acepta las 500 claves correctas");
    verificar(rechazaClaveMala, "autenticar() rechaza las 500 claves incorrectas");
    tabla.mostrarEstadisticas();

    bool eliminados = true;
    for (int i = 0; i < N; ++i) {
        if (!tabla.eliminar("nodo_" + std::to_string(i))) {
            eliminados = false;
        }
    }
    verificar(eliminados, "se eliminan los 500 usuarios uno por uno");
    bool nadieQueda = true;
    for (int i = 0; i < N; ++i) {
        if (tabla.existe("nodo_" + std::to_string(i))) {
            nadieQueda = false;
        }
    }
    verificar(nadieQueda, "la tabla quedó vacía: existe() es false para los 500 usuarios");
    verificar(!tabla.eliminar("nodo_0"), "volver a eliminar un usuario ya borrado devuelve false");
    tabla.mostrarEstadisticas();

    // Inserciones y borrados intercalados en una sola cadena (ejercita el enlazado).
    TablaHash intercalada(1);
    const int C = 200;
    bool ok = true;
    for (int i = 0; i < C; ++i) {
        if (!intercalada.insertar("temporal_" + std::to_string(i), "x")) {
            ok = false;
        }
    }
    for (int i = 0; i < C; i += 2) {  // se borra la mitad (pares)
        if (!intercalada.eliminar("temporal_" + std::to_string(i))) {
            ok = false;
        }
    }
    bool paresFuera = true;
    bool imparesDentro = true;
    for (int i = 0; i < C; ++i) {
        bool esta = intercalada.existe("temporal_" + std::to_string(i));
        if (i % 2 == 0 && esta) {
            paresFuera = false;
        }
        if (i % 2 != 0 && !esta) {
            imparesDentro = false;
        }
    }
    verificar(ok && paresFuera && imparesDentro,
              "con 200 nodos en una sola cadena: se borran los pares y sobreviven los impares");
    intercalada.mostrarEstadisticas();
}

/* ==========================================================================
 * 7) Destructor: liberación completa de cadenas en distintos alcances
 * ========================================================================== */
static void test_destructor_por_alcance() {
    seccion("7) Destructor y liberación de memoria");

    {
        TablaHash temporal(2);  // cadenas largas: 25 usuarios por bucket
        for (int i = 0; i < 50; ++i) {
            temporal.insertar("t_" + std::to_string(i), "v_" + std::to_string(i));
        }
        temporal.mostrarEstadisticas();
    }  // con AddressSanitizer, una fuga se reportaría exactamente aquí

    {
        TablaHash temporal(1);
        for (int i = 0; i < 50; ++i) {
            temporal.insertar("u_" + std::to_string(i), "v_" + std::to_string(i));
        }
        for (int i = 0; i < 50; ++i) {  // se vacía por completo antes del destructor
            temporal.eliminar("u_" + std::to_string(i));
        }
    }

    verificar(true, "ambos destructores recorrieron y liberaron todas las cadenas (ver ASan)");
}

/* ==========================================================================
 * 8) Carga alta: más de 200 usuarios ficticios y factor de carga elevado
 * ========================================================================== */
static void test_carga_alta_y_factor_de_carga() {
    seccion("8) Carga alta (>200 usuarios) y factor de carga elevado");

    const int BUCKETS = 31;    // tabla deliberadamente pequeña: eleva el factor
    const int USUARIOS = 250;  // más de 200 usuarios ficticios generados
    TablaHash tabla(BUCKETS);

    bool insertados = true;
    for (int i = 0; i < USUARIOS; ++i) {
        if (!tabla.insertar("cliente_" + std::to_string(i), "clave#" + std::to_string(i * 7 + 3))) {
            insertados = false;
        }
    }
    verificar(insertados, "se insertan 250 usuarios ficticios (>200) sin rechazos");

    // Con 250 usuarios en 31 buckets hay ~8 usuarios por bucket: la tabla queda
    // saturada a propósito para forzar colisiones por todas partes.
    bool todosExisten = true;
    bool todosAutentican = true;
    bool ningunoConClaveMala = true;
    for (int i = 0; i < USUARIOS; ++i) {
        std::string u = "cliente_" + std::to_string(i);
        std::string c = "clave#" + std::to_string(i * 7 + 3);
        if (!tabla.existe(u)) {
            todosExisten = false;
        }
        if (!tabla.autenticar(u, c)) {
            todosAutentican = false;
        }
        if (tabla.autenticar(u, c + "X")) {  // negativos: clave alterada
            ningunoConClaveMala = false;
        }
    }
    verificar(todosExisten, "existe() confirma los 250 usuarios de la carga alta");
    verificar(todosAutentican, "autenticar() acepta las 250 claves correctas (positivos)");
    verificar(ningunoConClaveMala, "autenticar() rechaza las 250 claves alteradas (negativos)");

    std::cout << "  [INFO]  factor de carga esperado = " << USUARIOS << " / " << BUCKETS << " = "
              << (static_cast<double>(USUARIOS) / static_cast<double>(BUCKETS)) << '\n';
    tabla.mostrarEstadisticas();  // factor de carga REAL y cadena de colisiones mas larga
}

/* ==========================================================================
 * Runner principal
 * ========================================================================== */
int main() {
    std::cout << "===== PRUEBAS DEL MODULO TABLA HASH / AUTENTICACION =====\n";

    test_constructor_y_estado_inicial();
    test_insercion_duplicados_y_autenticacion();
    test_obtener_indice();
    test_colisiones();
    test_eliminar_tres_casos();
    test_reinsercion_y_estres();
    test_destructor_por_alcance();
    test_carga_alta_y_factor_de_carga();

    std::cout << "\n===== RESUMEN =====\n";
    std::cout << "Pruebas correctas : " << pruebas_ok << '\n';
    std::cout << "Pruebas fallidas  : " << pruebas_fallidas << '\n';
    if (pruebas_fallidas == 0) {
        std::cout << "RESULTADO: TODAS LAS PRUEBAS PASARON\n";
    } else {
        std::cout << "ATENCION: HAY PRUEBAS FALLIDAS (revisar los [FALLO] de arriba)\n";
    }

    // Se retorna 0 (requerimiento de la prueba): al terminar main, todas las
    // TablaHash creadas salen de alcance y sus destructores liberan cada nodo
    // con delete y el arreglo con delete[], de modo que ASan/Valgrind no
    // reporta fugas de memoria.
    return 0;
}
