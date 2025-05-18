#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <commons/config.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>

// Estructura del buffer
typedef struct{
    uint32_t size;
    uint32_t offset;
    void* stream;
} t_buffer;

t_buffer *buffer_create(uint32_t size);

void buffer_add_stream(t_buffer *buffer, void *data, uint32_t size);

void buffer_add_uint32(t_buffer *buffer, uint32_t data);

void buffer_add_uint8(t_buffer *buffer, uint8_t data);

void buffer_add_string(t_buffer *buffer, char *string);

#endif