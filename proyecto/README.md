# Network OS — Sistema de archivos distribuido con enrutador de red

Proyecto universitario de **Estructuras de Datos**. Simula un *Network OS* que
integra un **árbol de directorios**, una **tabla hash propia** y un **grafo
ponderado** (Dijkstra, BFS/DFS) como enrutador de red, dejando registro de las
operaciones en `data/network_audit_log.txt`.

## Integrantes

<!-- Completar nombre y rama de cada integrante -->

| # | Nombre | Rama de Git |
|---|--------|-------------|
| 1 |        |             |
| 2 |        |             |
| 3 |        |             |
| 4 |        |             |

## Estructura de carpetas

```
proyecto/
├── .gitignore               -> ignora binarios (*.o/*.exe/network_os) y data/*.txt/*.log
├── Makefile                 -> g++ -std=c++17 -Wall -Wextra -g -fsanitize=address $(find src -name '*.cpp')
├── README.md
├── data/                    -> .csv de entrada (versionados) y el log generado en ejecución (no versionado)
│   ├── README.md
│   └── network_audit_log.txt (generado)
├── docs/                    -> documentación del equipo
│   └── README.md
├── src/
│   ├── main.cpp             -> crea SistemaArchivos + TablaHash + RedServidores, lanza menú, libera con delete
│   ├── Servidor.h           -> struct Servidor { nombre, SistemaArchivos archivos, TablaHash usuarios }
│   ├── auditoria/           -> escritura y lectura de data/network_audit_log.txt
│   │   ├── auditoria.hpp    -> AuditLogger::registrar / leerYMostrar / leer_log + RUTA_LOG_AUDITORIA
│   │   └── auditoria.cpp    -> append con [YYYY-MM-DD HH:MM:SS]
│   ├── estructuras/
│   │   ├── arbol/           -> árbol de directorios (crear, búsqueda recursiva, eliminación en cascada)
│   │   │   ├── arbol_directorios.hpp
│   │   │   └── arbol_directorios.cpp
│   │   ├── hash/            -> tabla hash propia (Horner 31 + encadenamiento)
│   │   │   ├── tabla_hash.hpp
│   │   │   └── tabla_hash.cpp
│   │   ├── grafo/           -> algoritmos sobre IGrafo (Dijkstra, BFS)
│   │   │   ├── dijkstra.hpp/.cpp
│   │   │   └── recorridos.hpp/.cpp
│   │   └── redservidores/   -> grafo oficial : public IGrafo (listas de adyacencia manuales)
│   │       ├── IGrafo.h + IAlgoritmoRuta.h
│   │       ├── ResultadoRuta.h/.cpp + ColaCircular.h/.cpp
│   │       ├── BusquedaAnchura.h/.cpp + ImpresorRutas.h/.cpp
│   │       └── RedServidores.h/.cpp
│   ├── menu/                -> menú interactivo de consola (0-13)
│   │   ├── menu.hpp         -> struct ContextoSistema + mostrar_menu/ejecutar_menu
│   │   └── menu.cpp
│   └── output/              -> binarios locales de pruebas (no versionar)
├── tests/                   -> pruebas por módulo (compilar con -fsanitize=address)
│   ├── README.md
│   ├── test_arbol.cpp       -> suite SistemaArchivos (incluye estrés 500 niveles + cascada masiva)
│   ├── test_hash.cpp        -> suite TablaHash (incluye estrés 2000 colisiones)
│   ├── test_grafo.cpp       -> suite RedServidores 100 nodos + 500 ops + aislado BFS
│   └── test_estres.cpp      -> auditoría 3000 registros en orden + resumen global
└── network_os.exe           -> (generado, no versionado)
```

## Cómo compilar y ejecutar

Requisitos: `g++` con soporte de **C++17** (probado con GCC 13).

Compilar (parado en la carpeta `proyecto/`):

```bash
g++ -std=c++17 -Wall -Wextra -Isrc -o network_os $(find src -name '*.cpp')
```

Ejecutar:

```bash
./network_os
```

> El binario y los logs generados en `data/` **no** se versionan (ver
> `.gitignore`); los `.csv` de entrada sí.

## Bitácora de Inteligencia Artificial

<!-- Sección reservada: documentar acá cada prompt usado con la IA
     (fecha, herramienta, prompt y qué se generó o aprovechó). -->
