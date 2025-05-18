#ifndef TEST_H_
#define TEST_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utils/buffer.h>
#include <utils/client.h>
#include <utils/opcodes.h>

typedef void* (*retornoPayload)(int);

typedef struct {
    uint32_t length;
    char* content;
} t_mensaje;

// Envia un mensaje desde un socket hacia otro. Servidor -> cliente, cliente -> servidor
void send_message(int socket, char* message);

// Recibe un payload de un socket cliente
void* recibir_payload(int socket_cliente);

// Extrae el mensaje contenido dentro del payload. Funcion de orden superior.
t_mensaje* extraer_mensaje(retornoPayload funcion);

// Libera la memoria del mensaje
void liberar_mensaje(t_mensaje* mensaje); 

#endif