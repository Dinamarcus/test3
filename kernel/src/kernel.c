#include <kernel.h>

t_listen_cpu *t_cpu_create(char* port, t_log* logger, char* server_name){
    t_listen_cpu *listener = malloc(sizeof(t_listen_cpu));
    listener->port = port;
    listener->logger = logger;
    listener->server_name = server_name;
    return listener;
}

// Servidor de escucacha DISPATCH de CPU
void start_cpu_dispatch_server(t_listen_cpu* data){
    int fd = start_server(data->port, data->logger, data->server_name);
    atender_clientes(fd, data->logger, (void*)&procesar_cliente);
}

// Servidor de escucacha INTERUPT de CPU
void start_cpu_interrupt_server(t_listen_cpu* data){
    int fd = start_server(data->port, data->logger, data->server_name);
    atender_clientes(fd, data->logger, (void*)&procesar_cliente);
}

// Servidor de escucacha IO
void start_io_server(t_listen_cpu* data){
    int fd = start_server(data->port, data->logger, data->server_name);
    atender_clientes(fd, data->logger, (void*)&procesar_cliente);
}