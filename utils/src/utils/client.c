#include <utils/client.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>

int create_conection(char *ip, char* puerto, char* server, t_log* logger){
	int err;
	struct addrinfo hints;
	struct addrinfo *server_info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	err = getaddrinfo(ip, puerto, &hints, &server_info);
	int socket_cliente = 0;
	socket_cliente = socket(
        server_info->ai_family, 
        server_info->ai_socktype,
        server_info -> ai_protocol
    );
	err = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
    if(err == -1){
        error_show("No se puedo establecer la conexion con el servidor : %s \n", server); 
        exit(EXIT_FAILURE);
    } else{
        log_info(logger, "Se efectuo la conexion al modulo solicitado de forma exitosa: %s", server);
    }
	freeaddrinfo(server_info);
	return socket_cliente;
}

void handshake_client(int socket_conexion, char* server, t_log* logger){
	int32_t handshake = 1;
	int32_t result;
	send(socket_conexion, &handshake, sizeof(int32_t), 0);
	recv(socket_conexion, &result, sizeof(int32_t), MSG_WAITALL);

	if (result == 0) {
		log_info(logger, "Handshake efectuado exitosamente");
		return;
	} else {
		log_error(logger, "Error recibiendo handshake");
        error_show("No se puedo establecer el handshake con el servidor : %s \n", server); 
		exit(EXIT_FAILURE);
	}
}

t_paquete* create_package(int op_code, t_buffer* buffer){
	t_paquete* package = malloc(sizeof(t_paquete));
	package->codigo_operacion = op_code;
	package->buffer = buffer;
	return package;
}

void* serialize_package(t_paquete* paquete){
    void * empaquetado = malloc(paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
    int offset = 0;
    memcpy(empaquetado + offset, &(paquete->codigo_operacion), sizeof(uint8_t)); // Aca enviamos el codigo de operacion
    offset += sizeof(uint8_t);
    memcpy(empaquetado + offset, &(paquete->buffer->size), sizeof(uint32_t)); // Aca enviamos el tamaño del buffer
    offset += sizeof(uint32_t);
    memcpy(empaquetado + offset, paquete->buffer->stream, paquete->buffer->size);
    return empaquetado;
}

int send_package(t_paquete* package, int client_socket){
	void* serilized_package = serialize_package(package);
	int result = send(client_socket, serilized_package, package->buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0);
	free(serilized_package);
	return result;
}

void buffer_delete(t_buffer *buffer){
    free(buffer->stream);
    free(buffer);
}

void delete_package(t_paquete* package){
	buffer_delete(package->buffer);
	free(package);
}