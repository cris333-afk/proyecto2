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
├── src/
│   ├── main.cpp                -> punto de entrada del programa
│   ├── estructuras/
│   │   ├── arbol/              -> árbol de directorios (crear, búsqueda
│   │   │                          recursiva, eliminación en cascada)
│   │   ├── hash/               -> tabla hash propia (función de dispersión
│   │   │                          manual + manejo de colisiones)
│   │   └── grafo/              -> grafo ponderado (Dijkstra, BFS/DFS)
│   ├── auditoria/              -> escritura y lectura de
│   │                              data/network_audit_log.txt
│   └── menu/                   -> menú interactivo de consola
├── data/                       -> .csv de entrada (versionados) y el log
│                                  generado en ejecución (no versionado)
├── tests/                      -> pruebas por módulo
├── docs/                       -> documentación del equipo
└── .gitignore
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
