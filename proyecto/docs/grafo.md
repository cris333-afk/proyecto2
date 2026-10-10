# Grafo de servidores

## Representación

La red es no dirigida y usa listas de adyacencia enlazadas. Cada `Arista`
guarda el destino, la latencia y el siguiente enlace.

```text
0 (A) ──10──> 1 (B)
1 (B) ──8───> 2 (C)
2 (C) ──7───> 3 (D)

0 ──25──> 2
```

Una conexión se agrega en las dos listas de adyacencia. Al eliminarla se
desvincula y libera una arista de cada extremo.

## Algoritmos

- Dijkstra guarda costos, predecesores y visitados; elige el menor costo con un
  recorrido lineal.
- BFS usa la cola circular propia para detectar servidores no alcanzables desde
  el servidor 0.

La red mantiene los nombres en un arreglo paralelo y el índice del servidor es
el mismo que se usa para ubicar su `Servidor` en el sistema integrado.
