#include <utils/opcodes.h>
#include <utils/buffer.h>
#include <utils/test.h>
#include <utils/server.h>

t_config* config;
t_log* logger;
int socket_servidor;

int main(int argc, char** argv) {

    logger = log_create("memoria.log", "memoria_main", 1, LOG_LEVEL_INFO);
    config = config_create("../memoria/memoria.config");

    if(config == NULL){
        log_error(logger, "El path del memoria.config es incorrecto");
        exit(EXIT_FAILURE);
    }

    // Cargar configuracion
    char *port = config_get_string_value(config, "PUERTO_ESCUCHA");

    socket_servidor = start_server(port, logger, "MEMORIA");
    
    atender_clientes(socket_servidor, logger, (void*)&procesar_cliente);
    
    printf("\ntest\n");

    return 0;                       
}