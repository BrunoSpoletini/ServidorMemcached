# ServidorMemcached

Memcached  es un sistema de memoria caché con pares claves-valor accesible por la red. Los clientes pueden conectarse a la misma y ejecutar comandos para agregar pares a la caché, buscar el valor asociado a una clave, borrar una entrada, etc. Tiene uso extensivo en sistemas distribuídos de gran escala, por ejemplo para cachear consultas a una base de datos (siempre que sea aceptable tener un valor un poco desactualizado).
El objetivo de este trabajo proyecto es implementar un memcached propio con funcionalidad relativamente completa. Las características principales que posee son:

- Soportar un protocolo de texto, que puede usarse desde, por ejemplo, netcat.
- Soportar un protocolo binario para datos que no sean texto.
- Proveer estadísticas sobre su uso.
- Tener un límite de memoria configurable que debe respetarse haciendo que el demonio “olvide” valores viejos si no hay suficiente memoria.
- Soportar multi-threading: la caché debe correr un hilo por cada hilo de hardware disponible y atender pedidos en simultáneo lo más posible.

Todo esto priorizando la eficiencia y robustez.


## Mejoras posibles:
  - Cambiar malloc por tryalloc en socket_handeler.c
  - Cambiar NOFILE limit en el main por un valor configurable
  - Realizar pruebas para analizar carga maxima soportada
