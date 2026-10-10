# Tabla hash

## Representación

`TablaHash` reserva un arreglo dinámico de punteros. Cada bucket apunta a una
cadena de `NodoHash` enlazada para resolver colisiones.

```text
buckets[0] ──> [usuarioA] ──> nullptr
buckets[1] ──> [usuarioB] ──> [usuarioC] ──> nullptr
buckets[2] ──> nullptr
```

## Dispersión

El índice se calcula con la regla de Horner y el multiplicador primo 31:

```text
h = (h * 31 + carácter) % capacidad
```

La búsqueda solo recorre la cadena del bucket obtenido; no recorre todos los
buckets. `cantidad` permite mostrar el factor de carga, y la longitud máxima de
una cadena muestra cómo afecta la dispersión.
