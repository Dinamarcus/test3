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
#include<kernel.h>

char * ip_memoria;
char *memory_port, *cpu_dispatch_port, *cpu_interrupt_port, *io_port;
int socket_memoria;
t_log* logger;
t_config* config;
pthread_t tid[3];


int main(int argc, char** argv) {
    
    logger = log_create("kernel.log", "kernel_main", 1, LOG_LEVEL_INFO);
    config = config_create("../kernel/kernel.config");

    if(config == NULL){
        log_error(logger, "El path del kernel.config es incorrecto");
        exit(EXIT_FAILURE);
    }
    
    // Cargar configuracion
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	memory_port = config_get_string_value(config, "PUERTO_MEMORIA");
    cpu_dispatch_port = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
	cpu_interrupt_port = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
	io_port = config_get_string_value(config, "PUERTO_ESCUCHA_IO");

    // Enviar mensaje a MEMORIA
    socket_memoria = create_conection(ip_memoria, memory_port, "KERNEL -> MEMORIA", logger);
    handshake_client(socket_memoria, "KERNEL -> MEMORIA", logger);
    send_message(socket_memoria, "SALUDOS DESDE EL KERNEL HACIA LA MEMORIA!");

    // Iniciar server dispatch, para escuchar desde la CPU
    t_listen_cpu* listener_dispatch_cpu = t_cpu_create(cpu_dispatch_port, logger, "SERVER CPU DISPATCH");
    if ((pthread_create(&(tid[0]), NULL, (void*)start_cpu_dispatch_server, (void*)listener_dispatch_cpu)) != 0){
    	printf("\nHubo un problema iniciando el servidor -> %s", "SERVER CPU DISPATCH");
    }

    // Iniciar server interrupt, para escuchar desde la CPU
    t_listen_cpu* listener_interrupt_cpu = t_cpu_create(cpu_interrupt_port, logger, "SERVER CPU INTERRUPT");
    if ((pthread_create(&(tid[1]), NULL, (void*)start_cpu_interrupt_server, (void*)listener_interrupt_cpu)) != 0){
    	printf("\nHubo un problema iniciando el servidor -> %s", "SERVER CPU INTERRUPT");
    }

    // Iniciar server para la escucha de modulos de I/O
    t_listen_cpu* listener_io = t_cpu_create(io_port, logger, "SERVER I/O");
    if ((pthread_create(&(tid[2]), NULL, (void*)start_io_server, (void*)listener_io)) != 0){
    	printf("\nHubo un problema iniciando el servidor -> %s", "SERVER I/O");
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);

    return 0;

}
