#include <utils/buffer.h>
#include <utils/opcodes.h>
#include <utils/client.h>
#include <utils/server.h>
#include <utils/test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>
#include<pthread.h>

char * ip_kernel;
char *kernel_port;
int socket_kernel;
t_log* logger;
t_config* config;

int main(int argc, char* argv[]) {
    
    logger = log_create("io.log", "io_main", 1, LOG_LEVEL_INFO);
    config = config_create("../io/io.config");

    if(config == NULL){
        log_error(logger, "El path del io.config es incorrecto");
        exit(EXIT_FAILURE);
    }

    // Cargar configuracion
    ip_kernel = config_get_string_value(config, "IP_KERNEL");
	kernel_port = config_get_string_value(config, "PUERTO_KERNEL");

    //Enviar mensaje a KERNEL
    socket_kernel = create_conection(ip_kernel, kernel_port, "IO -> KEREL", logger);
    handshake_client(socket_kernel, "IO -> KEREL", logger);
    send_message(socket_kernel, "SALUDOS DESDE IO HACIA EL KERNEL!");


    return 0;
}
