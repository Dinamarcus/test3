#ifndef SERVER_H_
#define SERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/error.h>
#include <commons/log.h>
#include <commons/config.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <utils/buffer.h>
#include <unistd.h>
#include <utils/test.h>
#include <pthread.h>

typedef struct {
    int fd_conexion;
    t_log* logger;
} datos_thread_t;

// Función que recibe un puntero a entero (fd) y devuelve un puntero a void (puede ser cualquier tipo de dato).
typedef void* (*ProcesarRequestFunc)(int* fd);

// Inicia el servidor y devuelve el socket del servidor.
int start_server(char* puerto, t_log* logger, char* modulo); 

// Realiza el handshake con el cliente. El cliente debe enviar un mensaje de handshake y el servidor responde con un mensaje de handshake.
void handshake_server(datos_thread_t* datos);

// Recibe el opcode del cliente. El cliente envía un mensaje con el opcode y el servidor lo recibe.
int recibir_opcode(int socket_cliente);

// Procesa el cliente.
void procesar_cliente(datos_thread_t* datos);

// Espera una nueva conexión de un cliente. El servidor está escuchando en el socket y espera a que un cliente se conecte.
int esperar_nueva_conexion_cliente(int socket_servidor, t_log* logger);

// Permite la conexion de multiples clientes en simultaneo, mediante la creacion de un hilo por cada nueva conexion.
void atender_clientes(int socket_servidor, t_log* logger,ProcesarRequestFunc procesar_request);

#endif