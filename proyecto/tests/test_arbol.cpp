/* ==========================================================================
 * Proyecto    : Network OS — Sistema de archivos distribuido con enrutador
 * Materia     : Estructuras de Datos
 * Módulo      : Árbol de directorios — PRUEBAS AISLADAS
 * Archivo     : tests/test_arbol.cpp
 * Descripción : Pruebas AISLADAS del módulo SistemaArchivos. No depende de
 *               ningún otro módulo del proyecto (ni AuditLogger, TablaHash,
 *               grafo, menú, autenticación o red). Contiene pruebas
 *               automáticas con reporte PASS/FAIL y un modo interactivo
 *               para ejercitar el módulo manualmente.
 *
 * Ejecutar:
 *   ./test_arbol.exe
 *
 * Autor       : (Liseth Briones)
 * ========================================================================== */

#include <iostream>
#include <sstream>
#include <string>

#include "../src/estructuras/arbol/arbol_directorios.hpp"

// ---------------------------------------------------------------------------
// Contadores globales para el resumen de las pruebas automáticas.
// ---------------------------------------------------------------------------
int gTotal = 0;
int gPasadas = 0;
int gFallidas = 0;

// Registra el resultado de una comprobación e imprime PASS o FAIL.
void verificar(bool condicion, const std::string& descripcion) {
    gTotal++;
    if (condicion) {
        gPasadas++;
        std::cout << "PASS: " << descripcion << std::endl;
    } else {
        gFallidas++;
        std::cout << "FAIL: " << descripcion << std::endl;
    }
}

// Indica si un texto contiene otro (para revisar la salida de mostrar()).
bool contiene(const std::string& texto, const std::string& sub) {
    return texto.find(sub) != std::string::npos;
}

// Captura en un string todo lo que mostrar() imprime, sin modificar mostrar().
// Reubica temporalmente el buffer de cout y luego lo restaura.
void capturarMostrar(const SistemaArchivos& sistema, std::string& salida) {
    std::ostringstream buffer;
    std::streambuf* original = std::cout.rdbuf(buffer.rdbuf());
    sistema.mostrar();
    std::cout.rdbuf(original);
    salida = buffer.str();
}

// Lee una línea del usuario. Devuelve false si no hay entrada disponible
// (por ejemplo cuando la entrada está redirigida y se llega al EOF).
bool leerLinea(const std::string& mensaje, std::string& texto) {
    std::cout << mensaje;
    if (!std::getline(std::cin, texto)) {
        std::cout << std::endl;
        return false;
    }
    return true;
}

// Convierte una línea en número sin usar excepciones; devuelve -1 si no es
// un entero válido.
int leerOpcion() {
    std::string texto;
    if (!leerLinea("Opcion: ", texto)) {
        return -1;
    }
    if (texto.empty()) {
        return -1;
    }
    int valor = 0;
    for (size_t i = 0; i < texto.size(); ++i) {
        if (texto[i] < '0' || texto[i] > '9') {
            return -1;
        }
        valor = valor * 10 + (texto[i] - '0');
    }
    return valor;
}

// ===========================================================================
// SECCIÓN 1: PRUEBAS AUTOMÁTICAS
// ===========================================================================

// --- CREAR ---------------------------------------------------------------
// Casos 1 a 9 del enunciado.
void probarCrear() {
    std::cout << "\n--- PRUEBAS DE CREAR ---" << std::endl;

    SistemaArchivos sistema;
    verificar(sistema.contarNodos() == 1,
              "Crear: el arbol nuevo tiene 1 nodo (la raiz)");

    // 1. Crear una carpeta en "/".
    verificar(sistema.crear("/", "docs", true),
              "Crear: carpeta 'docs' creada en '/'");
    // 2. Crear un archivo en "/".
    verificar(sistema.crear("/", "leeme.txt", false),
              "Crear: archivo 'leeme.txt' creado en '/'");
    // 3. Crear carpetas anidadas.
    verificar(sistema.crear("/docs", "txt", true),
              "Crear: carpeta 'txt' creada dentro de '/docs'");
    verificar(sistema.crear("/docs/txt", "notas", true),
              "Crear: carpeta 'notas' creada dentro de '/docs/txt'");
    // 4. Crear archivos dentro de carpetas.
    verificar(sistema.crear("/docs", "manual.txt", false),
              "Crear: archivo 'manual.txt' creado dentro de '/docs'");
    verificar(sistema.crear("/docs/txt", "notas.txt", false),
              "Crear: archivo 'notas.txt' creado dentro de '/docs/txt'");

    verificar(sistema.contarNodos() == 7,
              "Crear: el arbol tiene 7 nodos tras las creaciones validas");

    // 5. Rechazar nombre vacio.
    verificar(!sistema.crear("/", "", true),
              "Crear: rechaza nombre vacio");
    // 6. Rechazar ruta padre inexistente.
    verificar(!sistema.crear("/ruta-inexistente", "x", true),
              "Crear: rechaza ruta padre inexistente");
    // 7. Rechazar creacion dentro de un archivo.
    verificar(!sistema.crear("/leeme.txt", "hijo.txt", false),
              "Crear: rechaza crear dentro de un archivo");
    // 8. Rechazar nombre duplicado dentro del mismo padre.
    verificar(!sistema.crear("/docs", "manual.txt", false),
              "Crear: rechaza nombre duplicado en el mismo padre");
    // 9. Permitir el mismo nombre en padres diferentes.
    verificar(sistema.crear("/", "manual.txt", false),
              "Crear: permite el mismo nombre en padres diferentes");

    verificar(sistema.contarNodos() == 8,
              "Crear: los rechazos no aumentan el conteo (8 nodos)");
}

// --- BUSCAR --------------------------------------------------------------
// Casos 10 a 13 del enunciado.
void probarBuscar() {
    std::cout << "\n--- PRUEBAS DE BUSCAR ---" << std::endl;

    SistemaArchivos sistema;
    sistema.crear("/", "docs", true);
    sistema.crear("/docs", "a.txt", false);
    sistema.crear("/docs", "b.txt", false);
    sistema.crear("/", "otro", true);
    sistema.crear("/otro", "a.txt", false);  // nombre repetido

    // 10. Buscar un nodo existente.
    NodoArchivo* nodo = sistema.buscar("docs");
    verificar(nodo != nullptr, "Buscar: encuentra la carpeta 'docs'");
    verificar(nodo != nullptr && nodo->nombre == "docs",
              "Buscar: el nodo devuelto conserva el nombre buscado");
    verificar(nodo != nullptr && nodo->esCarpeta,
              "Buscar: 'docs' se reconoce como carpeta");

    NodoArchivo* archivo = sistema.buscar("a.txt");
    verificar(archivo != nullptr, "Buscar: encuentra el archivo 'a.txt'");
    verificar(archivo != nullptr && !archivo->esCarpeta,
              "Buscar: 'a.txt' se reconoce como archivo");
    verificar(archivo != nullptr && archivo->padre != nullptr &&
                  archivo->padre->nombre == "docs",
              "Buscar: el padre de la coincidencia de 'a.txt' es 'docs'");

    // 11. Buscar un nodo inexistente.
    verificar(sistema.buscar("no-existe.txt") == nullptr,
              "Buscar: devuelve nullptr si el nombre no existe");
    // 12. Buscar con nombre vacio.
    verificar(sistema.buscar("") == nullptr,
              "Buscar: devuelve nullptr con nombre vacio");
    // 13. Nombres repetidos en carpetas diferentes.
    NodoArchivo* repetido = sistema.buscar("a.txt");
    verificar(repetido != nullptr,
              "Buscar: encuentra 'a.txt' aunque este repetido en dos carpetas");
    verificar(repetido != nullptr && repetido->nombre == "a.txt",
              "Buscar: con nombres repetidos devuelve una coincidencia segun el recorrido");
    verificar(repetido != nullptr && repetido->primerHijo == nullptr,
              "Buscar: el archivo devuelto es una hoja (no tiene hijos)");

    std::cout << "NOTA: buscar() busca por NOMBRE y no por ruta. Con nombres" << std::endl;
    std::cout << "      repetidos devuelve la primera coincidencia del recorrido." << std::endl;
}

// --- ELIMINAR ------------------------------------------------------------
// Casos 14 a 16 del enunciado.
void probarEliminar() {
    std::cout << "\n--- PRUEBAS DE ELIMINAR ---" << std::endl;

    SistemaArchivos sistema;
    sistema.crear("/", "vacia", true);
    sistema.crear("/", "archivo.txt", false);
    sistema.crear("/", "conArchivos", true);
    sistema.crear("/conArchivos", "x.txt", false);

    verificar(sistema.contarNodos() == 5,
              "Eliminar: el arbol de prueba tiene 5 nodos");

    // 14. Eliminar un archivo.
    verificar(sistema.eliminar("/archivo.txt"),
              "Eliminar: elimina un archivo");
    verificar(sistema.contarNodos() == 4,
              "Eliminar: el conteo disminuye en 1 tras eliminar el archivo");
    verificar(sistema.buscar("archivo.txt") == nullptr,
              "Eliminar: el archivo eliminado ya no se encuentra");

    // 15. Eliminar una carpeta vacia.
    verificar(sistema.eliminar("/vacia"),
              "Eliminar: elimina una carpeta vacia");
    verificar(sistema.contarNodos() == 3,
              "Eliminar: la carpeta vacia se resta del conteo");
    verificar(sistema.buscar("vacia") == nullptr,
              "Eliminar: la carpeta vacia ya no se encuentra");

    // 16. Eliminar una carpeta con archivos.
    verificar(sistema.eliminar("/conArchivos"),
              "Eliminar: elimina una carpeta con un archivo dentro");
    verificar(sistema.contarNodos() == 1,
              "Eliminar: queda solo la raiz tras eliminar la carpeta con contenido");
    verificar(sistema.buscar("x.txt") == nullptr,
              "Eliminar: el archivo interno de la carpeta tambien desaparece");
}

// 17. Eliminar una carpeta con varios niveles de subdirectorios.
void probarEliminarNiveles() {
    std::cout << "\n--- ELIMINAR CON VARIOS NIVELES ---" << std::endl;

    SistemaArchivos sistema;
    sistema.crear("/", "a", true);
    sistema.crear("/a", "b", true);
    sistema.crear("/a/b", "c", true);
    sistema.crear("/a/b/c", "d.txt", false);
    sistema.crear("/a/b/c", "e.txt", false);

    verificar(sistema.contarNodos() == 6,
              "Eliminar: el arbol de 4 niveles tiene 6 nodos");
    verificar(sistema.eliminar("/a"),
              "Eliminar: elimina la carpeta raiz de la rama multinivel");
    verificar(sistema.contarNodos() == 1,
              "Eliminar: los 5 descendientes se liberaron en cascada");
    verificar(sistema.buscar("d.txt") == nullptr &&
                  sistema.buscar("e.txt") == nullptr,
              "Eliminar: los archivos del nivel mas profundo ya no existen");
}

// 18 y 19. No se puede eliminar la raiz y una ruta inexistente se rechaza.
void probarEliminarRaizYRutaInexistente() {
    std::cout << "\n--- ELIMINAR RAIZ Y RUTA INEXISTENTE ---" << std::endl;

    SistemaArchivos sistema;
    sistema.crear("/", "docs", true);
    sistema.crear("/docs", "a.txt", false);
    const int antes = sistema.contarNodos();

    // 18. Intentar eliminar "/" debe ser rechazado.
    verificar(!sistema.eliminar("/"),
              "Eliminar: rechaza eliminar la raiz '/'");
    verificar(sistema.contarNodos() == antes,
              "Eliminar: el conteo no cambia al intentar borrar la raiz");
    verificar(sistema.buscar("/") != nullptr,
              "Eliminar: la raiz sigue presente tras el intento");
    verificar(!sistema.eliminar("/") && sistema.contarNodos() == antes,
              "Eliminar: la raiz no puede eliminarse ni siquiera dos veces");

    // 19. Intentar eliminar una ruta inexistente.
    verificar(!sistema.eliminar("/no-existe"),
              "Eliminar: rechaza una ruta inexistente");
    verificar(!sistema.eliminar("/docs/no-existe.txt"),
              "Eliminar: rechaza un archivo inexistente dentro de una carpeta");
    verificar(!sistema.eliminar("/docs/a.txt/sub"),
              "Eliminar: rechaza una ruta que cuelga de un archivo");
    verificar(sistema.contarNodos() == antes,
              "Eliminar: las rutas inexistentes no alteran el conteo");
}

// 20, 21 y 22. Primer hijo, hijo intermedio y ultimo hijo de una lista.
void probarEliminarHermanos() {
    std::cout << "\n--- ELIMINAR HERMANOS (primerHijo / siguienteHermano) ---" << std::endl;

    SistemaArchivos sistema;
    sistema.crear("/", "h1", true);
    sistema.crear("/", "h2", true);
    sistema.crear("/", "h3", true);
    sistema.crear("/", "h4", true);

    verificar(sistema.contarNodos() == 5,
              "Hermanos: cuatro hermanos mas la raiz son 5 nodos");

    // 20. Primer hijo: el padre queda apuntando al siguiente hermano.
    verificar(sistema.eliminar("/h1"),
              "Hermanos: elimina el primer hijo");
    verificar(sistema.contarNodos() == 4,
              "Hermanos: el conteo baja a 4 tras quitar el primer hijo");
    verificar(sistema.buscar("h1") == nullptr,
              "Hermanos: h1 ya no existe");
    NodoArchivo* h2 = sistema.buscar("h2");
    verificar(h2 != nullptr && h2->siguienteHermano != nullptr &&
                  h2->siguienteHermano->nombre == "h3",
              "Hermanos: h2 pasa a ser el primer hijo y enlaza con h3");

    // 21. Hijo intermedio: el anterior enlaza directo con el siguiente.
    verificar(sistema.eliminar("/h3"),
              "Hermanos: elimina un hijo intermedio");
    verificar(sistema.contarNodos() == 3,
              "Hermanos: el conteo baja a 3 tras quitar el hijo intermedio");
    verificar(sistema.buscar("h3") == nullptr,
              "Hermanos: h3 ya no existe");
    h2 = sistema.buscar("h2");
    verificar(h2 != nullptr && h2->siguienteHermano != nullptr &&
                  h2->siguienteHermano->nombre == "h4",
              "Hermanos: h2 enlaza directo con h4 tras quitar h3");

    // 22. Ultimo hijo: el anterior queda con siguienteHermano nulo.
    verificar(sistema.eliminar("/h4"),
              "Hermanos: elimina el ultimo hijo");
    verificar(sistema.contarNodos() == 2,
              "Hermanos: el conteo baja a 2 tras quitar el ultimo hijo");
    h2 = sistema.buscar("h2");
    verificar(h2 != nullptr && h2->siguienteHermano == nullptr,
              "Hermanos: h2 queda como unico hijo con siguienteHermano nulo");
    std::cout << "Arbol resultante (solo la raiz y h2):" << std::endl;
    sistema.mostrar();
}

// --- ELIMINACIÓN EN CASCADA ---------------------------------------------
// Verifica el caso exacto del enunciado: 9 nodos antes, 3 despues.
void probarEliminacionEnCascada() {
    std::cout << "\n--- PRUEBAS DE ELIMINACION EN CASCADA ---" << std::endl;

    SistemaArchivos sistema;
    // /
    //   documentos/
    //     universidad/
    //       tarea1.txt
    //       tarea2.txt
    //     personal/
    //       notas.txt
    //   fotos/
    //     viaje.png
    sistema.crear("/", "documentos", true);
    sistema.crear("/documentos", "universidad", true);
    sistema.crear("/documentos/universidad", "tarea1.txt", false);
    sistema.crear("/documentos/universidad", "tarea2.txt", false);
    sistema.crear("/documentos", "personal", true);
    sistema.crear("/documentos/personal", "notas.txt", false);
    sistema.crear("/", "fotos", true);
    sistema.crear("/fotos", "viaje.png", false);

    verificar(sistema.contarNodos() == 9,
              "Cascada: antes de eliminar hay 9 nodos");

    std::cout << "Arbol antes de eliminar:" << std::endl;
    sistema.mostrar();

    verificar(sistema.eliminar("/documentos"),
              "Cascada: elimina '/documentos'");

    verificar(sistema.contarNodos() == 3,
              "Cascada: despues de eliminar /documentos quedan 3 nodos");

    // Los seis descendientes de /documentos ya no deben encontrarse.
    verificar(sistema.buscar("documentos") == nullptr,
              "Cascada: 'documentos' ya no se encuentra");
    verificar(sistema.buscar("universidad") == nullptr,
              "Cascada: 'universidad' ya no se encuentra");
    verificar(sistema.buscar("tarea1.txt") == nullptr,
              "Cascada: 'tarea1.txt' ya no se encuentra");
    verificar(sistema.buscar("tarea2.txt") == nullptr,
              "Cascada: 'tarea2.txt' ya no se encuentra");
    verificar(sistema.buscar("personal") == nullptr,
              "Cascada: 'personal' ya no se encuentra");
    verificar(sistema.buscar("notas.txt") == nullptr,
              "Cascada: 'notas.txt' ya no se encuentra");

    // La rama /fotos debe intacta.
    verificar(sistema.buscar("fotos") != nullptr,
              "Cascada: 'fotos' sigue existiendo");
    verificar(sistema.buscar("viaje.png") != nullptr,
              "Cascada: 'viaje.png' sigue existiendo");

    std::string salida;
    capturarMostrar(sistema, salida);
    verificar(salida == "/\n  fotos/\n    viaje.png\n",
              "Cascada: el arbol resultante es /, fotos/ y viaje.png");

    std::cout << "Arbol despues de eliminar:" << std::endl;
    sistema.mostrar();
}

// --- MOSTRAR -------------------------------------------------------------
// Casos 23 a 28 del enunciado.
void probarMostrar() {
    std::cout << "\n--- PRUEBAS DE MOSTRAR ---" << std::endl;

    // 23. Arbol recien creado.
    SistemaArchivos vacio;
    std::string salidaVacia;
    capturarMostrar(vacio, salidaVacia);
    verificar(salidaVacia == "/\n",
              "Mostrar: un arbol recien creado muestra unicamente '/'");

    // 24, 25 y 26. Arbol con varios niveles, archivos y carpetas.
    SistemaArchivos sistema;
    sistema.crear("/", "docs", true);
    sistema.crear("/docs", "tarea1.txt", false);
    sistema.crear("/", "fotos", true);
    sistema.crear("/fotos", "viaje.png", false);
    sistema.crear("/", "proyectos", true);
    sistema.crear("/proyectos", "universidad", true);
    sistema.crear("/proyectos/universidad", "proyecto.cpp", false);

    std::string salida;
    capturarMostrar(sistema, salida);
    std::cout << "--- Salida de mostrar() ---" << std::endl;
    std::cout << salida;

    const std::string esperada =
        "/\n"
        "  docs/\n"
        "    tarea1.txt\n"
        "  fotos/\n"
        "    viaje.png\n"
        "  proyectos/\n"
        "    universidad/\n"
        "      proyecto.cpp\n";

    verificar(salida == esperada,
              "Mostrar: la salida coincide exactamente con el arbol esperado");
    // 28. La raiz se muestra como "/" y no como "//".
    verificar(!contiene(salida, "//"),
              "Mostrar: la raiz se muestra como '/' y nunca como '//'");
    // 27. Las carpetas llevan "/" y los archivos no.
    verificar(contiene(salida, "  docs/\n"),
              "Mostrar: la carpeta tiene sangria de 2 espacios y '/' al final");
    verificar(contiene(salida, "    tarea1.txt\n"),
              "Mostrar: el archivo tiene sangria de 4 espacios y sin '/'");
    // 26. Dos espacios por nivel.
    verificar(contiene(salida, "      proyecto.cpp\n"),
              "Mostrar: el nivel 3 usa 6 espacios de sangria");
    verificar(!contiene(salida, "\n  tarea1.txt\n"),
              "Mostrar: los archivos no comparten sangria con su carpeta padre");
    verificar(contiene(salida, "\n  fotos/\n"),
              "Mostrar: los hermanos comparten el mismo nivel de sangria");
}

// --- CONTAR NODOS --------------------------------------------------------
// Casos 29 a 32 del enunciado.
void probarContarNodos() {
    std::cout << "\n--- PRUEBAS DE CONTAR NODOS ---" << std::endl;

    // 29. Arbol recien creado: 1 nodo (la raiz esta incluida).
    SistemaArchivos sistema;
    verificar(sistema.contarNodos() == 1,
              "contarNodos: el arbol recien creado tiene 1 nodo (incluye la raiz)");

    // 30. El conteo aumenta correctamente con cada creacion.
    sistema.crear("/", "a", true);            // 2
    sistema.crear("/", "b", true);            // 3
    sistema.crear("/a", "c.txt", false);      // 4
    sistema.crear("/a", "sub", true);         // 5
    sistema.crear("/a/sub", "d.txt", false);  // 6
    sistema.crear("/a/sub", "e.txt", false);  // 7
    verificar(sistema.contarNodos() == 7,
              "contarNodos: cuenta 7 nodos tras las creaciones");

    // 31. Disminuye en 1 al eliminar un archivo.
    sistema.eliminar("/a/sub/d.txt");
    verificar(sistema.contarNodos() == 6,
              "contarNodos: disminuye en 1 tras eliminar un archivo");

    // 32. Disminuye por la cantidad total al eliminar una carpeta con hijos.
    // "/a" contiene a, c.txt, sub y e.txt: son 4 nodos. Quedan la raiz y "b".
    sistema.eliminar("/a");
    verificar(sistema.contarNodos() == 2,
              "contarNodos: disminuye 4 nodos al eliminar '/a' con sus descendientes");
    verificar(sistema.buscar("b") != nullptr,
              "contarNodos: 'b' sigue existiendo en la raiz tras eliminar '/a'");

    // La raiz nunca se elimina durante la vida del objeto.
    verificar(!sistema.eliminar("/") && sistema.contarNodos() == 2,
              "contarNodos: la raiz permanece tras rechazar su eliminacion");
}

// --- DESTRUCTOR -----------------------------------------------------------
// Nunca se llama manualmente a ~SistemaArchivos(): se usan bloques de alcance
// para que el destructor se ejecute automáticamente al salir del bloque.
// La verificacion de memoria la hace AddressSanitizer al terminar el proceso:
// si el destructor no liberara todo el arbol, ASan reportaria la fuga.
void probarDestructor() {
    std::cout << "\n--- PRUEBAS DEL DESTRUCTOR ---" << std::endl;

    // 1. Arbol pequeno: lo libera el destructor al salir del bloque.
    {
        SistemaArchivos sistema;
        sistema.crear("/", "docs", true);
        sistema.crear("/docs", "a.txt", false);
        verificar(sistema.contarNodos() == 3,
                  "Destructor: arbol pequeno construido con 3 nodos");
    }
    verificar(true,
              "Destructor: el arbol pequeno se libero al salir del bloque (ASan debe reportar 0 fugas)");

    // 2. Arbol multinivel con archivos y carpetas.
    {
        SistemaArchivos sistema;
        sistema.crear("/", "documentos", true);
        sistema.crear("/documentos", "universidad", true);
        sistema.crear("/documentos/universidad", "tarea1.txt", false);
        sistema.crear("/documentos/universidad", "tarea2.txt", false);
        sistema.crear("/documentos", "personal", true);
        sistema.crear("/documentos/personal", "notas.txt", false);
        sistema.crear("/", "fotos", true);
        sistema.crear("/fotos", "viaje.png", false);
        verificar(sistema.contarNodos() == 9,
                  "Destructor: arbol multinivel construido con 9 nodos");
    }
    verificar(true,
              "Destructor: el arbol multinivel se libero al salir del bloque");

    // 3. Se eliminan subarboles antes de destruir; el destructor libera el resto.
    {
        SistemaArchivos sistema;
        sistema.crear("/", "documentos", true);
        sistema.crear("/documentos", "universidad", true);
        sistema.crear("/documentos/universidad", "tarea1.txt", false);
        sistema.crear("/", "fotos", true);
        sistema.crear("/fotos", "viaje.png", false);
        sistema.crear("/", "temporal", true);

        sistema.eliminar("/documentos");       // libera 3 nodos
        sistema.eliminar("/fotos/viaje.png");  // libera 1 nodo
        sistema.eliminar("/temporal");         // libera 1 nodo
        verificar(sistema.contarNodos() == 2,
                  "Destructor: quedan 2 nodos tras eliminar subarboles previos");
        verificar(sistema.buscar("tarea1.txt") == nullptr,
                  "Destructor: los nodos ya eliminados no se vuelven a eliminar");
    }
    verificar(true,
              "Destructor: los nodos restantes se liberaron sin doble liberacion");

    std::cout << "NOTA: estas comprobaciones se validan ejecutando el test con" << std::endl;
    std::cout << "      AddressSanitizer; un fallo del destructor se reportaria" << std::endl;
    std::cout << "      como fuga o double-free al terminar el proceso." << std::endl;
}

// ===========================================================================
// SECCIÓN 2: MODO INTERACTIVO
// ===========================================================================

// Muestra los datos basicos de un nodo devuelto por buscar().
void mostrarInfoNodo(const NodoArchivo* nodo) {
    std::cout << "  Nombre       : " << nodo->nombre << std::endl;
    std::cout << "  Tipo         : " << (nodo->esCarpeta ? "carpeta" : "archivo")
              << std::endl;
    std::cout << "  Padre        : "
              << (nodo->padre != nullptr ? nodo->padre->nombre
                                         : "(ninguno, es la raiz)")
              << std::endl;
    std::cout << "  Tiene hijos  : "
              << (nodo->primerHijo != nullptr ? "si" : "no") << std::endl;
    std::cout << "  Hermano sig. : "
              << (nodo->siguienteHermano != nullptr ? nodo->siguienteHermano->nombre
                                                    : "(ninguno)")
              << std::endl;
}

// Muestra el menu de casos limite. Los casos que necesitan una estructura
// concreta usan un arbol temporal propio para no alterar el arbol del usuario;
// los que solo consultan datos usan el arbol actual.
void menuCasosLimite(SistemaArchivos& sistema) {
    while (true) {
        std::cout << "\n========== CASOS LÍMITE ==========" << std::endl;
        std::cout << "\n1. Nombre vacío" << std::endl;
        std::cout << "2. Ruta inexistente" << std::endl;
        std::cout << "3. Crear dentro de un archivo" << std::endl;
        std::cout << "4. Nombre duplicado" << std::endl;
        std::cout << "5. Buscar nombre inexistente" << std::endl;
        std::cout << "6. Buscar nombre vacío" << std::endl;
        std::cout << "7. Eliminar ruta inexistente" << std::endl;
        std::cout << "8. Intentar eliminar /" << std::endl;
        std::cout << "9. Eliminar carpeta vacía" << std::endl;
        std::cout << "10. Eliminar carpeta con contenido" << std::endl;
        std::cout << "11. Nombres repetidos en diferentes carpetas" << std::endl;
        std::cout << "12. Eliminar primer hijo" << std::endl;
        std::cout << "13. Eliminar hijo intermedio" << std::endl;
        std::cout << "14. Eliminar último hijo" << std::endl;
        std::cout << "0. Volver" << std::endl;

        const int opcion = leerOpcion();
        if (opcion == 0) {
            return;
        }
        if (opcion == -1) {
            std::cout << "  Entrada no valida." << std::endl;
            if (std::cin.eof()) {
                return;
            }
            continue;
        }

        switch (opcion) {
        case 1: {
            std::cout << "\n[1] Crear con nombre vacio sobre el arbol actual" << std::endl;
            const int antes = sistema.contarNodos();
            if (sistema.crear("/", "", true)) {
                std::cout << "  Resultado: ACEPTADO (inesperado, debe rechazarse)" << std::endl;
            } else {
                std::cout << "  Resultado: RECHAZADO correctamente" << std::endl;
            }
            std::cout << "  Nodos antes/despues: " << antes << " / "
                      << sistema.contarNodos() << std::endl;
            break;
        }
        case 2: {
            std::cout << "\n[2] Crear en una ruta padre inexistente" << std::endl;
            if (sistema.crear("/ruta-que-no-existe", "x", true)) {
                std::cout << "  Resultado: ACEPTADO (inesperado, debe rechazarse)" << std::endl;
            } else {
                std::cout << "  Resultado: RECHAZADO correctamente" << std::endl;
            }
            std::cout << "  Nodos totales: " << sistema.contarNodos() << std::endl;
            break;
        }
        case 3: {
            std::cout << "\n[3] Crear dentro de un archivo (arbol de prueba)" << std::endl;
            SistemaArchivos prueba;
            prueba.crear("/", "archivo.txt", false);
            prueba.mostrar();
            if (prueba.crear("/archivo.txt", "hijo.txt", false)) {
                std::cout << "  Resultado: ACEPTADO (inesperado, debe rechazarse)" << std::endl;
            } else {
                std::cout << "  Resultado: RECHAZADO correctamente" << std::endl;
            }
            std::cout << "  Nodos totales: " << prueba.contarNodos()
                      << " (deben quedar 2: la raiz y el archivo)" << std::endl;
            break;
        }
        case 4: {
            std::cout << "\n[4] Nombre duplicado en el mismo padre (arbol de prueba)" << std::endl;
            SistemaArchivos prueba;
            prueba.crear("/", "dup", true);
            prueba.crear("/dup", "interno.txt", false);
            if (prueba.crear("/", "dup", true)) {
                std::cout << "  Resultado: ACEPTADO (inesperado, debe rechazarse)" << std::endl;
            } else {
                std::cout << "  Resultado: RECHAZADO correctamente" << std::endl;
            }
            std::cout << "  Nodos totales: " << prueba.contarNodos()
                      << " (deben quedar 3)" << std::endl;
            prueba.mostrar();
            break;
        }
case 5: {
            std::cout << "\n[5] Buscar un nombre inexistente en el arbol actual" << std::endl;
            NodoArchivo* nodo = sistema.buscar("este-nombre-no-existe-xyz");
            if (nodo == nullptr) {
                std::cout << "  Resultado: no se encontro (correcto)" << std::endl;
            } else {
                std::cout << "  Resultado: se encontro por error '" << nodo->nombre << "'"
                          << std::endl;
            }
            break;
        }
        case 6: {
            std::cout << "\n[6] Buscar con nombre vacio en el arbol actual" << std::endl;
            if (sistema.buscar("") == nullptr) {
                std::cout << "  Resultado: devuelve nullptr (correcto)" << std::endl;
            } else {
                std::cout << "  Resultado: devolvio un nodo (inesperado)" << std::endl;
            }
            break;
        }
        case 7: {
            std::cout << "\n[7] Eliminar una ruta inexistente del arbol actual" << std::endl;
            const int antes = sistema.contarNodos();
            if (sistema.eliminar("/ruta-inexistente-xyz")) {
                std::cout << "  Resultado: ACEPTADO (inesperado, debe rechazarse)" << std::endl;
            } else {
                std::cout << "  Resultado: RECHAZADO correctamente" << std::endl;
            }
            std::cout << "  Nodos antes/despues: " << antes << " / "
                      << sistema.contarNodos() << std::endl;
            break;
        }
        case 8: {
            std::cout << "\n[8] Intentar eliminar la raiz '/'" << std::endl;
            const int antes = sistema.contarNodos();
            if (sistema.eliminar("/")) {
                std::cout << "  Resultado: ACEPTADO (inesperado, debe rechazarse)" << std::endl;
            } else {
                std::cout << "  Resultado: RECHAZADO correctamente" << std::endl;
            }
            std::cout << "  Nodos antes/despues: " << antes << " / "
                      << sistema.contarNodos() << std::endl;
            break;
        }
        case 9: {
            std::cout << "\n[9] Eliminar una carpeta vacia (arbol de prueba)" << std::endl;
            SistemaArchivos prueba;
            prueba.crear("/", "vacia", true);
            prueba.crear("/", "otra", true);
            prueba.mostrar();
            if (prueba.eliminar("/vacia")) {
                std::cout << "  Resultado: eliminada correctamente" << std::endl;
            } else {
                std::cout << "  Resultado: rechazada (inesperado)" << std::endl;
            }
            std::cout << "  Nodos restantes: " << prueba.contarNodos()
                      << " (deben quedar 2)" << std::endl;
            prueba.mostrar();
            break;
        }
        case 10: {
            std::cout << "\n[10] Eliminar una carpeta con contenido (arbol de prueba)" << std::endl;
            SistemaArchivos prueba;
            prueba.crear("/", "docs", true);
            prueba.crear("/docs", "a.txt", false);
            prueba.crear("/docs", "sub", true);
            prueba.crear("/docs/sub", "b.txt", false);
            prueba.mostrar();
            if (prueba.eliminar("/docs")) {
                std::cout << "  Resultado: eliminada en cascada correctamente" << std::endl;
            } else {
                std::cout << "  Resultado: rechazada (inesperado)" << std::endl;
            }
            std::cout << "  Nodos restantes: " << prueba.contarNodos()
                      << " (debe quedar 1, solo la raiz)" << std::endl;
            prueba.mostrar();
            break;
        }
        case 11: {
            std::cout << "\n[11] Nombres repetidos en carpetas diferentes (arbol de prueba)"
                      << std::endl;
            SistemaArchivos prueba;
            prueba.crear("/", "d1", true);
            prueba.crear("/", "d2", true);
            const bool primero = prueba.crear("/d1", "a.txt", false);
            const bool segundo = prueba.crear("/d2", "a.txt", false);
            prueba.mostrar();
            std::cout << "  Primer 'a.txt' creado  : " << (primero ? "si" : "no") << std::endl;
            std::cout << "  Segundo 'a.txt' creado : " << (segundo ? "si" : "no")
                      << " (debe ser 'si': son padres distintos)" << std::endl;
            NodoArchivo* nodo = prueba.buscar("a.txt");
            std::cout << "  buscar(\"a.txt\") devuelve la primera coincidencia por nombre:"
                      << std::endl;
            if (nodo != nullptr) {
                mostrarInfoNodo(nodo);
            } else {
                std::cout << "  (no encontrado)" << std::endl;
            }
            break;
        }
case 12: {
            std::cout << "\n[12] Eliminar el primer hijo (arbol de prueba)" << std::endl;
            SistemaArchivos prueba;
            prueba.crear("/", "h1", true);
            prueba.crear("/", "h2", true);
            prueba.crear("/", "h3", true);
            std::cout << "  Antes:" << std::endl;
            prueba.mostrar();
            if (prueba.eliminar("/h1")) {
                std::cout << "  Resultado: h1 (primer hijo) eliminado" << std::endl;
            }
            std::cout << "  Despues (" << prueba.contarNodos() << " nodos):" << std::endl;
            prueba.mostrar();
            std::cout << "  h1 todavia existe? " << (prueba.buscar("h1") != nullptr ? "si" : "no")
                      << std::endl;
            break;
        }
        case 13: {
            std::cout << "\n[13] Eliminar un hijo intermedio (arbol de prueba)" << std::endl;
            SistemaArchivos prueba;
            prueba.crear("/", "h1", true);
            prueba.crear("/", "h2", true);
            prueba.crear("/", "h3", true);
            std::cout << "  Antes:" << std::endl;
            prueba.mostrar();
            if (prueba.eliminar("/h2")) {
                std::cout << "  Resultado: h2 (intermedio) eliminado" << std::endl;
            }
            std::cout << "  Despues (" << prueba.contarNodos() << " nodos):" << std::endl;
            prueba.mostrar();
            NodoArchivo* h1 = prueba.buscar("h1");
            std::cout << "  h1 sigue enlazado con h3? "
                      << ((h1 != nullptr && h1->siguienteHermano != nullptr &&
                           h1->siguienteHermano->nombre == "h3") ? "si" : "no")
                      << std::endl;
            break;
        }
        case 14: {
            std::cout << "\n[14] Eliminar el ultimo hijo (arbol de prueba)" << std::endl;
            SistemaArchivos prueba;
            prueba.crear("/", "h1", true);
            prueba.crear("/", "h2", true);
            prueba.crear("/", "h3", true);
            std::cout << "  Antes:" << std::endl;
            prueba.mostrar();
            if (prueba.eliminar("/h3")) {
                std::cout << "  Resultado: h3 (ultimo hijo) eliminado" << std::endl;
            }
            std::cout << "  Despues (" << prueba.contarNodos() << " nodos):" << std::endl;
            prueba.mostrar();
            NodoArchivo* h2 = prueba.buscar("h2");
            std::cout << "  h2 queda con siguienteHermano nulo? "
                      << ((h2 != nullptr && h2->siguienteHermano == nullptr) ? "si" : "no")
                      << std::endl;
            break;
        }
        default:
            std::cout << "  Opcion no valida." << std::endl;
        }
    }
}

// Modo interactivo: el usuario introduce datos que se aplican realmente
// sobre un SistemaArchivos, sin depender de ningun otro modulo.
void modoInteractivo() {
    SistemaArchivos sistema;
    std::cout << "\nEl arbol comienza con la raiz '/'." << std::endl;

    while (true) {
        std::cout << "\n========== MODO INTERACTIVO ==========" << std::endl;
        std::cout << "\n1. Crear carpeta" << std::endl;
        std::cout << "2. Crear archivo" << std::endl;
        std::cout << "3. Buscar" << std::endl;
        std::cout << "4. Mostrar árbol" << std::endl;
        std::cout << "5. Eliminar" << std::endl;
        std::cout << "6. Contar nodos" << std::endl;
        std::cout << "7. Casos límite" << std::endl;
        std::cout << "0. Salir" << std::endl;

        const int opcion = leerOpcion();
        if (opcion == 0) {
            std::cout << "\nSaliendo del modo interactivo." << std::endl;
            std::cout << "~SistemaArchivos() liberara todo el arbol automaticamente." << std::endl;
            return;
        }
        if (opcion == -1) {
            std::cout << "  Entrada no valida." << std::endl;
            if (std::cin.eof()) {
                std::cout << "\nFin de la entrada. Cerrando el modo interactivo."
                          << std::endl;
                return;
            }
            continue;
        }

        switch (opcion) {
        case 1: {
            std::string ruta;
            std::string nombre;
            if (!leerLinea("Ruta del padre: ", ruta)) { return; }
            if (!leerLinea("Nombre: ", nombre)) { return; }
            if (sistema.crear(ruta, nombre, true)) {
                std::cout << "  Carpeta '" << nombre << "' creada en '" << ruta << "'"
                          << std::endl;
            } else {
                std::cout << "  Operacion RECHAZADA (ruta inexistente, el padre no es"
                          << std::endl;
                std::cout << "  carpeta, nombre vacio o nombre duplicado en ese padre)"
                          << std::endl;
            }
            std::cout << "  Nodos totales: " << sistema.contarNodos() << std::endl;
            break;
        }
        case 2: {
            std::string ruta;
            std::string nombre;
            if (!leerLinea("Ruta del padre: ", ruta)) { return; }
            if (!leerLinea("Nombre: ", nombre)) { return; }
            if (sistema.crear(ruta, nombre, false)) {
                std::cout << "  Archivo '" << nombre << "' creado en '" << ruta << "'"
                          << std::endl;
            } else {
                std::cout << "  Operacion RECHAZADA (ruta inexistente, el padre no es"
                          << std::endl;
                std::cout << "  carpeta, nombre vacio o nombre duplicado en ese padre)"
                          << std::endl;
            }
            std::cout << "  Nodos totales: " << sistema.contarNodos() << std::endl;
            break;
        }
        case 3: {
            std::string nombre;
            if (!leerLinea("Nombre a buscar: ", nombre)) { return; }
            NodoArchivo* nodo = sistema.buscar(nombre);
            if (nodo != nullptr) {
                std::cout << "  Nodo encontrado:" << std::endl;
                mostrarInfoNodo(nodo);
            } else {
                std::cout << "  No se encontro ningun nodo con ese nombre." << std::endl;
            }
            break;
        }
        case 4:
            std::cout << "\n--- Arbol actual ---" << std::endl;
            sistema.mostrar();
            break;
        case 5: {
            std::string ruta;
            if (!leerLinea("Ruta completa: ", ruta)) { return; }
            if (sistema.eliminar(ruta)) {
                std::cout << "  Eliminada correctamente la ruta '" << ruta << "'" << std::endl;
            } else {
                std::cout << "  Operacion RECHAZADA (ruta inexistente o es la raiz '/')"
                          << std::endl;
            }
            std::cout << "  Nodos totales: " << sistema.contarNodos() << std::endl;
            break;
        }
        case 6:
            std::cout << "  Total de nodos (incluida la raiz): " << sistema.contarNodos()
                      << std::endl;
            break;
        case 7:
            menuCasosLimite(sistema);
            break;
        default:
            std::cout << "  Opcion no valida." << std::endl;
        }
    }
}

// ===========================================================================
// MAIN
// ===========================================================================
int main() {
    std::cout << "=== PRUEBAS AISLADAS: MODULO ARBOL DE DIRECTORIOS ===" << std::endl;
    std::cout << "Clase probada: SistemaArchivos" << std::endl;
    std::cout << "Sin dependencias de otros modulos del proyecto." << std::endl;

    std::cout << "\n>> PRUEBAS AUTOMATICAS <<" << std::endl;

    probarCrear();
    probarBuscar();
    probarEliminar();
    probarEliminarNiveles();
    probarEliminarRaizYRutaInexistente();
    probarEliminarHermanos();
    probarEliminacionEnCascada();
    probarMostrar();
    probarContarNodos();
    probarDestructor();

    std::cout << "\n================================" << std::endl;
    std::cout << "RESUMEN DE PRUEBAS" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "Pruebas ejecutadas: " << gTotal << std::endl;
    std::cout << "PASS: " << gPasadas << std::endl;
    std::cout << "FAIL: " << gFallidas << std::endl;

    std::cout << "\nDeseas entrar al MODO INTERACTIVO? (s/n): " << std::endl;
    std::string respuesta;
    if (std::getline(std::cin, respuesta)) {
        if (!respuesta.empty() && (respuesta[0] == 's' || respuesta[0] == 'S')) {
            modoInteractivo();
        } else {
            std::cout << "\nNo se entra al modo interactivo." << std::endl;
        }
    }

    // Al terminar, los destructores ya se ejecutaron. Si quedara algun nodo
    // sin liberar, AddressSanitizer lo reportaria como fuga en este punto.
    return gFallidas == 0 ? 0 : 1;
}