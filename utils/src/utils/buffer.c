#include <utils/buffer.h>
#include <sys/socket.h>

// Creacion de un buffer inicial
t_buffer *buffer_create(uint32_t size){
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = size;
    buffer->offset = 0;
    buffer->stream = malloc(size);
    return buffer;
}

// Agregar un stream al buffer
void buffer_add_stream(t_buffer *buffer, void *data, uint32_t size){
    memcpy(buffer->stream + buffer->offset, data, size);
    buffer->offset += size;
}

// Agregar un dato del tipo uint32_t al buffer
void buffer_add_uint32(t_buffer *buffer, uint32_t data){
    buffer_add_stream(buffer, &data, sizeof(uint32_t));
}

// Agregar un dato del tipo uint8_t al buffer
void buffer_add_uint8(t_buffer *buffer, uint8_t data){
    buffer_add_stream(buffer, &data, sizeof(uint8_t));
}

// Agregar un string al buffer
void buffer_add_string(t_buffer *buffer, char *string){
    buffer_add_uint32(buffer, (strlen(string)+1) );
    buffer_add_stream(buffer, string, (strlen(string)+1));
}