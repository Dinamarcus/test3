#include <utils/test.h>

void send_message(int socket, char* message){
    // Se genera la estructura del mensaje
    t_mensaje mensaje;
    mensaje.content = message;
    mensaje.length = strlen(message) + 1;
    // Se crea el buffer
    t_buffer* buffer = buffer_create(sizeof(uint32_t) + mensaje.length );
    // Se agrega el mensaje al buffer, en este caso es solo el string de la cadena
    // en otro caso pueden agregarse mas datos al stream
    buffer_add_string(buffer, mensaje.content);
    // Se empaquea el mensaje junto al OP CODE
    t_paquete* paquete = create_package(TEST_COMUNICACIONAL, buffer); 
    send_package(paquete, socket);
    // Se libera la memoria del HEAP
    delete_package(paquete);
}

void* recibir_payload(int socket_cliente) {
    //t_paquete* paquete = malloc(sizeof(t_paquete));
    t_buffer* buffer = malloc(sizeof(t_buffer));

    // Primero recibimos el codigo de operacion
    //recv(unSocket, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

    // Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
    recv(socket_cliente, &(buffer->size), sizeof(uint32_t), 0);
    buffer->stream = malloc(buffer->size);
    recv(socket_cliente, buffer->stream, buffer->size, 0);

    // printf("Tamaño de paquete : %d" ,buffer->tamanio);

    void* stream = buffer->stream;

    return stream;
}

t_mensaje* extraer_mensaje(retornoPayload funcion) {
    t_mensaje* mensaje = malloc(sizeof(t_mensaje));
    void* payload = funcion;

    // Por último, para obtener el nombre, primero recibimos el tamaño y luego el texto en sí:
    memcpy(&(mensaje->length), payload, sizeof(uint32_t));
    payload += sizeof(uint32_t);
    // printf("Tamaño del mensaje : %d" , mensaje->mensaje_length);

    mensaje->content = malloc(mensaje->length);
    memcpy(mensaje->content, payload, mensaje->length);
    // printf("mensaje : %s" , mensaje->mensaje);

    return mensaje;
}

void liberar_mensaje(t_mensaje* mensaje) {    
    free(mensaje->content);
    free(mensaje);
}