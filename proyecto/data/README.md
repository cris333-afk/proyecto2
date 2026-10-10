# data/

Los tres archivos versionados son la entrada inicial del programa:

- `servidores.csv`: un nombre por línea. El orden asigna los ids 0, 1, 2, ...
- `conexiones.csv`: `origen,destino,latencia_ms`.
- `rutas.csv`: `servidor,ruta_padre,nombre,tipo`, donde `tipo` vale 1 para carpeta y 0 para archivo.

`main.cpp` los carga en ese orden al iniciar. Si falta un archivo, el programa lo informa y continúa para que el menú siga disponible.

Las líneas que empiezan con `#` son comentarios. El log `network_audit_log.txt` se genera durante la ejecución y no se versiona.
