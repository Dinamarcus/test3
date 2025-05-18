#include <utils/server.h>

int start_server(char* puerto, t_log* logger, char* modulo){
    int socket_servidor, err;
	struct addrinfo hints, *servinfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, puerto, &hints, &servinfo);
	// Creamos el socket de escucha del servidor
	socket_servidor = socket(
        servinfo->ai_family,
        servinfo->ai_socktype,
        servinfo->ai_protocol
    );    
    if ( setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int) ) < 0){
		error_show("Error seteando la configuracion del socket del modulo %s", modulo);
		exit(EXIT_FAILURE);
	}
	// Asociamos el socket a un puerto
	err = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes
	err = listen(socket_servidor, SOMAXCONN);
	freeaddrinfo(servinfo);    
    if(err == -1){
        error_show("Error iniciando el servidor de escucha del modulo %s", modulo);
        exit(EXIT_FAILURE);
    } else{
        log_info(logger, "Listo para escuchar el siguiente modulo: %s", modulo);
    }
	return socket_servidor;
}

void handshake_server(datos_thread_t* datos){
    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;
    recv(datos->fd_conexion, &handshake, sizeof(int32_t), MSG_WAITALL);
    if (handshake == 1) {
		log_info(datos->logger, "Handshake recibido");
        send(datos->fd_conexion, &resultOk, sizeof(int32_t), 0);
    } else {
		log_error(datos->logger, "Handshake Error");
        send(datos->fd_conexion, &resultError, sizeof(int32_t), 0);
    }
}

int recibir_opcode(int socket_cliente){
	uint8_t cod_op;

	if(recv(socket_cliente, &cod_op, sizeof(uint8_t), MSG_WAITALL) > 0){
		return cod_op;
    }else{
		close(socket_cliente);
		return -1;
	}
}

void procesar_cliente(datos_thread_t* datos){

    handshake_server(datos);
    int iterador = 1;

    while (iterador) {
		int cod_op = recibir_opcode(datos->fd_conexion);
        t_mensaje* mensaje = extraer_mensaje(recibir_payload(datos->fd_conexion));

		switch (cod_op) {
            case TEST_COMUNICACIONAL:
                printf("ok\n");

                // Mostrar el buffer recibido
                printf("\n------------------------\n");
                printf("Tamanio de mensaje: %d\n", mensaje->length);
                printf("Mensaje del buffer: %s\n", mensaje->content);

                liberar_mensaje(mensaje);
                break;
            case -1:
                iterador = 0;
                printf("El cliente se desconecto\n");
                break;
		}
	} 
    
}

int esperar_nueva_conexion_cliente(int socket_servidor, t_log* logger)
{
	// Aceptamos un nuevo cliente
	int socket_cliente;
	socket_cliente = accept(socket_servidor, NULL, NULL);

    if(socket_cliente == -1){
        error_show("Error en la espera del cliente");
        exit(EXIT_FAILURE);
    } else{
        log_info(logger, "Se conecto un cliente!");
    }

	return socket_cliente;
}

void atender_clientes(int socket_servidor, t_log* logger, ProcesarRequestFunc procesar_request) {
    while (true) {
        pthread_t thread;
        
        // En lugar de crear solo un puntero a int, crea la estructura
        datos_thread_t* datos = malloc(sizeof(datos_thread_t));
        if (datos == NULL) {
            // Manejo de error
            log_error(logger, "Error al asignar memoria para datos_thread_t");
            continue;
        }
        
        // Obtener el descriptor de conexión
        int fd_conexion = esperar_nueva_conexion_cliente(socket_servidor, logger);
        
        // Asignar los datos a la estructura
        datos->fd_conexion = fd_conexion;
        datos->logger = logger;  // Usar el logger que recibiste como parámetro
        
        // Crear el hilo pasando la estructura completa
        pthread_create(&thread,
                      NULL,
                      (void*) procesar_request,
                      (void*) datos);
        
        pthread_detach(thread);
    }
}