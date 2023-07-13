# ServidorMemcached

Servidor de claves valor con soporte para multi-threading, que maneja solicitudes a traves de un protocolo binario y un protocolo de texto.

Mejoras pendientes:
  - Cambiar malloc por tryalloc en socket_handeler.c
  - Cambiar NOFILE limit en el main por un valor configurable
  - Realizar pruebas para analizar carga maxima soportada
