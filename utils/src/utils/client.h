#ifndef CLIENT_H_
#define CLIENT_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/error.h>
#include <commons/log.h>
#include <string.h>
#include <utils/buffer.h>
#include <sys/socket.h>

typedef struct {
    uint8_t codigo_operacion;
    t_buffer* buffer;
} t_paquete;

int create_conection(char *ip, char* puerto, char* server, t_log* logger);

void handshake_client(int socket_conexion, char* server, t_log* logger);

t_paquete* create_package(int op_code, t_buffer* buffer);

void* serialize_package(t_paquete* paquete);

int send_package(t_paquete* package, int client_socket);

void buffer_delete(t_buffer *buffer);

void delete_package(t_paquete* package);

#endif