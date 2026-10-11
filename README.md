## Bitacora IA

### Entrada 1 — (extensión Cline en VS Code)

**Para qué:** reconocer el repositorio antes de tocar nada y no romper el trabajo de mis compañeras.

*Objetivo propuesto:* pedirle que inspeccionara la estructura real del proyecto, el `README.md`, `main.cpp` y los esqueletos existentes, y me dijera dónde encajaba mi módulo y qué convenciones seguía el equipo.

*Qué obtuve:* el mapa real del proyecto (raíz en `proyecto/`, carpeta `src/estructuras/hash/`, carpeta `tests/` ya existente, estilo de encabezados de documentación y guardas de inclusión `PROYECTO_*_HPP`).

*Qué revisé yo:* que mi módulo no alterara `main.cpp` ni el resto de módulos; confirmé que la carpeta destino era la del esqueleto `tabla_hash.*` y que `AuditLogger` todavía no estaba subido, así que necesitaba una forma de compilar aislado.

### Entrada 2 

**Para qué:** fijar el contrato de la clase y justificar las decisiones algorítmicas antes de escribir código.

*Objetivo propuesto:* respetar firmas ya definidas (constructor, destructor, `insertar`, `existe`, `autenticar`, `eliminar`, `obtenerIndice`, `mostrarEstadisticas`), sin contenedores de la STL y con manejo de memoria manual.

*Qué obtuve:* propuesta de arreglo dinámico de punteros más nodos enlazados propios, colisiones por encadenamiento simple, dispersión polinomial (regla de Horner con el primo 31 y módulo aplicado en cada iteración) y capacidad por defecto 101 por ser primo.

*Qué revisé yo:* por qué el 31 es un buen multiplicador y por qué la capacidad es un número primo; también por qué el módulo en cada iteración evita el desbordamiento con nombres de usuario largos. Esto quedó preparado para explicarlo en la defensa oral.

### Entrada 3 — 

**Para qué:** implementar el módulo completo, sobre todo la parte delicada: borrado y liberación de memoria.

*Qué obtuve:* código comentado en español explicando cada decisión — inserción al inicio de la cadena para lograr O(1), búsqueda acotada a la cadena del bucket, reconexión del puntero predecesor **antes** de aplicar `delete` (los tres casos: cabeza, nodo intermedio y último), guardar el `siguiente` antes de liberar cada nodo en el destructor y `delete[]` del arreglo.

*Qué revisé yo:* recorrí el código línea por línea, lo compilé con `-Wall -Wextra -pedantic` sin una sola advertencia y verifiqué dos comportamientos que me importaban: que un usuario duplicado **no** sobrescriba la clave original y que borrar no rompa el resto de la cadena.

### Entrada 4 

**Para qué:** demostrar que el módulo es robusto con un script de pruebas propio.

*Objetivo propuesto:* un script independiente con su propio `main()` que insertara más de 200 usuarios ficticios para elevar el factor de carga, forzara colisiones explícitas entre usuarios distintos, validara la autenticación en casos positivos y negativos, probara los tres casos de eliminación dentro de una cadena de colisión, imprimiera las estadísticas y terminara con `return 0` para que el destructor limpiara todo.

*Qué obtuve:* una suite con mini-marco de aserciones propio (sin librerías externas, para no contradecir la regla de "sin STL" del módulo), 8 secciones y líneas `[INFO]` que muestran el índice y los usuarios que de verdad colisionan (p. ej. 30 usuarios distintos compartiendo el mismo bucket).

*Qué revisé yo:* la ejecuté (73 verificaciones correctas, 0 fallas, código de salida 0) y ajusté la separación explícita de casos positivos y negativos de `autenticar`, además de una cadena de colisión controlada (`cuarto → tercero → segundo → primero`) para poder explicar en la defensa qué nodo se está borrando en cada caso.

### Entrada 5 

**Para qué:** demostrar que no hay fugas de memoria y entender los límites de la verificación.

*Qué obtuve:* un arnés temporal (fuera del repositorio) que sobrecarga los operadores `new`/`delete` globales para comparar asignaciones contra liberaciones en cinco escenarios: destructor con 500 nodos en cadenas, borrado parcial, tabla vacía al destruir, duplicados rechazados y casos borde. Resultado: **2311 asignaciones = 2311 liberaciones**.

*Qué corregí yo / límites asumidos:* al principio el enlace con `-fsanitize=address` fallaba porque en este Windows/MinGW no existe el runtime de ASan (`cannot find -lasan`), así que descarté esa vía aquí y dejé anotado en el archivo de pruebas que la validación con AddressSanitizer y Valgrind se hará en Linux/WSL. Preferí ser explícito en eso antes que dejar una afirmación que no pude comprobar.

### Entrada 6 

**Para qué:** dejar el módulo dentro de los archivos que el equipo ya tenía como referencia y limpiar duplicados.

*Qué obtuve:* el traslado del contenido a `tabla_hash.hpp` y `tabla_hash.cpp` (conservando la guarda `PROYECTO_TABLA_HASH_HPP` y el encabezado de documentación del proyecto), la actualización del `#include` en el script de pruebas y la eliminación de `TablaHash.h` y `TablaHash.cpp`, verificando que no quedara ninguna referencia colgando.

*Qué corregí yo:* al mover el código detecté un fallo real de integración — el esqueleto `tabla_hash.hpp` no incluía `<string>`, por lo que el compilador rechazaba las declaraciones; agregué el include y volví a compilar. Después recompilé el proyecto completo (los 8 archivos `.cpp` de `src/`), volví a correr las pruebas y en git registré el borrado de `TablaHash.cpp`, que ya había quedado en un commit de mi rama, para no dejar archivos huérfanos.
