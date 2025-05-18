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

char *ip_memory, *ip_kernel;
char *memory_port, *dispatch_port, *interrupt_port;
int socket_dispatch, socket_interrupt, socket_memory;

t_log* logger;
t_config* config;

int main(int argc, char* argv[]) {
    
    logger = log_create("cpu.log", "cpu_main", 1, LOG_LEVEL_INFO);
    config = config_create("../cpu/cpu.config");

    if(config == NULL){
        log_error(logger, "El path del cpu.config es incorrecto");
        exit(EXIT_FAILURE);
    }

    // Cargar configuracion
    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    ip_memory = config_get_string_value(config, "IP_MEMORIA");
    memory_port = config_get_string_value(config, "PUERTO_MEMORIA");
    dispatch_port = config_get_string_value(config, "PUERTO_KERNEL_DISPATCH");
    interrupt_port = config_get_string_value(config, "PUERTO_KERNEL_INTERRUPT");
    

    //Enviar mensaje a KERNEL DISPATCH
    socket_dispatch = create_conection(ip_kernel, dispatch_port, "CPU -> DISPATCH KERNEL", logger);
    handshake_client(socket_dispatch, "CPU -> DISPATCH KERNEL", logger);
    send_message(socket_dispatch, "SALUDOS DESDE CPU HACIA EL DISPATCH DE KERNEL!");

    //Enviar mensaje a KERNEL INTERRUPT
    socket_interrupt = create_conection(ip_kernel, interrupt_port, "CPU -> INTERRUPT KERNEL", logger);
    handshake_client(socket_interrupt, "CPU -> INTERRUPT KERNEL", logger);
    send_message(socket_interrupt, "SALUDOS DESDE CPU HACIA EL INTERRUPT DE KERNEL!");

    //Enviar mensaje a MEMORIA
    socket_memory = create_conection(ip_memory, memory_port, "CPU -> MEMORY", logger);
    handshake_client(socket_memory, "CPU -> MEMORY", logger);
    send_message(socket_memory, "SALUDOS DESDE CPU HACIA MEMORY!");

    return 0;
}
