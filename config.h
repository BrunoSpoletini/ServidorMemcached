#ifndef __CONFIG_H_
#define __CONFIG_H_

/* Configuración del servidor MEMCHACHED */

/* Básico */
// Número de threads que se crearán para manejar solicitudes de clientes
#define DEFAULT_MAX_THREADS 4

// Número de socket de texto por defecto
#define DEFAULT_TEXT_SOCKET 888

// Número de socket binario por defecto
#define DEFAULT_BIN_SOCKET 889

// Limite de memoria del servidor por defecto
#define DEFAULT_MEM_LIMIT 600*1000000ull  ///  6GB

// Tamaño de la tabla hash
#define TABLESIZE 50000

/* Avanzado */
// Numero máximo de eventos que devolverá epoll_wait
#define MAX_EVENTS 500000
// Numero máximo de clientes que pueden estar esperando para conectarse al servidor
#define MAX_CLIENTS_QUEUE 10000

#endif