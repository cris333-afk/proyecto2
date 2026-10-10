# Network OS — Sistema de archivos distribuido con enrutador de red

Proyecto universitario de **Estructuras de Datos**. Simula un *Network OS* que
integra un árbol de directorios, una tabla hash propia y un grafo ponderado
(Dijkstra y BFS) como enrutador de red.

## Descripción

Cada servidor de la red tiene su propio sistema de archivos y su propia tabla
de usuarios. El menú permite seleccionar el servidor activo; las operaciones de
archivos y usuarios se aplican solo a ese servidor. Las conexiones y las rutas
se calculan sobre el grafo completo.

El sistema registra las operaciones en `data/network_audit_log.txt` y libera los
recursos con `delete`/`delete[]` al salir.

## Integrantes

| # | Nombre | Rama de Git |
|---|--------|-------------|
| 1 | Cris | `cristofer` |
| 2 | Liseth | `Arbol-de-Directorios-Liseth` |
| 3 | Angel | `Angel` |
| 4 | Cesar | `Cesar` |

## Estructura de carpetas

```text
proyecto/
├── .gitignore
├── Makefile
├── README.md
├── data/
│   ├── README.md
│   ├── servidores.csv       -> nombres y orden de ids
│   ├── conexiones.csv       -> origen,destino,latencia_ms
│   ├── rutas.csv             -> servidor,ruta_padre,nombre,tipo
│   └── network_audit_log.txt (generado, no versionado)
├── docs/
│   ├── README.md
│   ├── arbol-directorios.md
│   ├── tabla-hash.md
│   └── grafo.md
├── src/
│   ├── main.cpp              -> carga inicial, integración y liberación
│   ├── Servidor.h            -> nombre + árbol + tabla de usuarios
│   ├── auditoria/            -> AuditLogger y bitácora
│   ├── estructuras/
│   │   ├── arbol/            -> SistemaArchivos
│   │   ├── hash/             -> TablaHash
│   │   ├── grafo/            -> Dijkstra y BFS
│   │   └── redservidores/    -> RedServidores y estructuras auxiliares
│   └── menu/                 -> ContextoSistema y menú interactivo
└── tests/                    -> pruebas por módulo
```

## Carga inicial

Al iniciar, `main.cpp` carga en este orden:

1. `data/servidores.csv`: cada línea es un nombre; el orden define el id.
2. `data/conexiones.csv`: cada línea usa `origen,destino,latencia_ms`.
3. `data/rutas.csv`: cada línea usa `servidor,ruta_padre,nombre,tipo`.

Las líneas que empiezan con `#` son comentarios. Si un archivo falta, el
programa informa el problema y permite continuar usando el menú.

## Compilación y ejecución

### Linux

El `Makefile` usa C++17, advertencias y AddressSanitizer:

```bash
make
./network_os
```

El comando equivalente, sin GNU Make, es:

```bash
g++ -std=c++17 -Wall -Wextra -g -fsanitize=address $(find src -name '*.cpp') -o network_os
./network_os
```

### Windows (MinGW/GCC)

El comando `find` no existe en Windows. Desde `proyecto/`, en PowerShell:

```powershell
$fuentes = Get-ChildItem -Path src -Filter *.cpp -Recurse | ForEach-Object { $_.FullName }
g++ -std=c++17 -Wall -Wextra -g -o network_os.exe $fuentes
.\network_os.exe
```

El entorno Windows usa GCC 16.1.0. La distribución MinGW usada por el equipo no
incluye el runtime de AddressSanitizer, por eso la compilación de Windows no
usa `-fsanitize=address`. En Linux sí debe ejecutarse con ASan y terminar sin
reportes de fugas.

Para limpiar los artefactos de Linux:

```bash
make clean
```

## Pruebas

Las pruebas de cada módulo están en `tests/`. Los comandos exactos de compilación
de cada prueba están comentados al inicio de cada archivo.

## Documentación

Los diagramas de representación y las decisiones de memoria están en:

- [Árbol de directorios](docs/arbol-directorios.md)
- [Tabla hash](docs/tabla-hash.md)
- [Grafo de servidores](docs/grafo.md)

## Bitácora de Inteligencia Artificial

<!-- Sección reservada: documentar acá cada prompt usado con la IA
     (fecha, herramienta, prompt y qué se generó o aprovechó). -->
