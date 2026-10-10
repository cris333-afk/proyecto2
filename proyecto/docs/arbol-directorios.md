# Árbol de directorios

## Representación

Cada `NodoArchivo` guarda su nombre, si es carpeta y tres punteros:

```text
NodoArchivo
├── nombre
├── esCarpeta
├── primerHijo       ──────┐
├── siguienteHermano ──────┼─── lista de hermanos
└── padre             ─────┘
```

La lista de hijos no usa `vector`: `primerHijo` apunta al primer hijo y
`siguienteHermano` enlaza el resto de los hijos del mismo padre.

## Ejemplo

```text
/
├── docs/
│   ├── tarea1.txt
│   └── tarea2.txt
└── fotos/
    └── viaje.png
```

## Operación de eliminación

La eliminación es en postorden: se guarda `siguienteHermano` antes de borrar el
hijo, se recorren sus descendientes y finalmente se ejecuta `delete` sobre el
nodo actual. Así no se intenta usar un puntero después de liberar su objeto.
