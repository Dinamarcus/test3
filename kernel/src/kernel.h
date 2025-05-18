#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/server.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>

typedef struct{
    char* port;
    t_log* logger;
    char* server_name;
} t_listen_cpu;

t_listen_cpu *t_cpu_create(char* port, t_log* logger, char* server_name);

void start_cpu_dispatch_server(t_listen_cpu* data);

void start_cpu_interrupt_server(t_listen_cpu* data);

void start_io_server(t_listen_cpu* data);

#endif