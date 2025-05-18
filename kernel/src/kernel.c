#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

t_log* logger;
t_config* config;
char* ip_memoria;
int puerto_memoria;
int puerto_escucha_dispatch;
int puerto_escucha_interrupt;
int puerto_escucha_io;
char* algoritmo_corto_plazo;
char* algoritmo_ingreso_a_ready;
double alfa;
int estimacion_inicial;
int tiempo_suspension;

// Variables globales
t_queue* cola_new;        // Cola de procesos NEW
t_list* lista_ready;      // Lista de procesos READY
t_list* lista_exec;       // Lista de procesos en EXEC
t_list* lista_blocked;    // Lista de procesos bloqueados
t_list* lista_exit;       // Lista de procesos finalizados

pthread_mutex_t mutex_new;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_exec;
pthread_mutex_t mutex_blocked;
pthread_mutex_t mutex_exit;

sem_t sem_procesos_new;
sem_t sem_procesos_ready;

int pid_contador = 0;
bool planificador_activo = false;

// Estados de proceso
typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCKED,
    EXIT
} t_estado_proceso;

// Estructura PCB
typedef struct {
    int pid;                    // Identificador único del proceso
    int pc;                     // Program Counter
    char* nombre_archivo;       // Nombre del archivo pseudocódigo
    int tamanio;                // Tamaño del proceso
    t_estado_proceso estado;    // Estado actual
    int contador_estados[5];    // Cantidad de veces en cada estado
    long tiempo_estados[5];     // Tiempo acumulado en cada estado
    struct timespec inicio_estado; // Tiempo de inicio del estado actual
} t_pcb;

void inicializar_planificador() {
    // Crear colas y listas
    cola_new = queue_create();
    lista_ready = list_create();
    lista_exec = list_create();
    lista_blocked = list_create();
    lista_exit = list_create();
    
    // Inicializar mutex
    pthread_mutex_init(&mutex_new, NULL);
    pthread_mutex_init(&mutex_ready, NULL);
    pthread_mutex_init(&mutex_exec, NULL);
    pthread_mutex_init(&mutex_blocked, NULL);
    pthread_mutex_init(&mutex_exit, NULL);
    
    // Inicializar semáforos
    sem_init(&sem_procesos_new, 0, 0);
    sem_init(&sem_procesos_ready, 0, 0);
    
    
    // Inicializar contador de PIDs
    pid_contador = 0;
    
    // Esperar Enter para iniciar la planificación
    printf("Presione Enter para iniciar la planificación...\n");
    getchar();
    
    planificador_activo = true;
    log_info(logger, "Planificador activado");
    
}

void* recibir_conexiones_cpu(void* arg) {
    // Implementación para recibir conexiones de CPU
    // Por ahora dejamos esto vacío para el Checkpoint 2
    while(1) {
        sleep(1);
    }
    return NULL;
}

void* recibir_conexiones_io(void* arg) {
    // Implementación para recibir conexiones de IO
    // Por ahora dejamos esto vacío para el Checkpoint 2
    while(1) {
        sleep(1);
    }
    return NULL;
}

// Solicitar memoria para un proceso
bool solicitar_memoria_para_proceso(t_pcb* pcb) {
    // Aquí implementar la comunicación con el módulo Memoria
    // Para el Checkpoint 2, podemos simular la respuesta
    
    log_info(logger, "## Solicitando memoria para el proceso %d", pcb->pid);
    
    // Simulación: enviar solicitud a Memoria y recibir respuesta
    // En un escenario real, esto debería conectarse con el módulo Memoria
    
    // Por ahora, simulamos que siempre hay espacio
    return true;
}

void inicializar_conexiones() {
    // Iniciar servidor para CPU
    pthread_t hilo_servidor_cpu;
    pthread_create(&hilo_servidor_cpu, NULL, recibir_conexiones_cpu, NULL);
    
    // Iniciar servidor para IO
    pthread_t hilo_servidor_io;
    pthread_create(&hilo_servidor_io, NULL, recibir_conexiones_io, NULL);
    
    // Intentar conectar con Memoria (solo para verificar que esté disponible)
    // Por ahora no implementamos esto para el Checkpoint 2
    
    log_info(logger, "Conexiones inicializadas correctamente");
}

void inicializar_kernel() {
    // Inicializar logger
    logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    log_info(logger, "Iniciando Kernel...");
    
    if(logger == NULL){
        log_error(logger, "El path del kernel.log es incorrecto");
    }

    // Cargar configuración
    config = config_create("kernel.config");

    if(config == NULL){
        log_error(logger, "El path del kernel.config es incorrecto");
    }


    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    
    puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
    puerto_escucha_dispatch = config_get_int_value(config, "PUERTO_ESCUCHA_DISPATCH");
    puerto_escucha_interrupt = config_get_int_value(config, "PUERTO_ESCUCHA_INTERRUPT");
    puerto_escucha_io = config_get_int_value(config, "PUERTO_ESCUCHA_IO");
    algoritmo_corto_plazo = config_get_string_value(config, "ALGORITMO_CORTO_PLAZO");
    algoritmo_ingreso_a_ready = config_get_string_value(config, "ALGORITMO_INGRESO_A_READY");
    alfa = config_get_double_value(config, "ALFA");
    estimacion_inicial = config_get_int_value(config, "ESTIMACION_INICIAL");
    tiempo_suspension = config_get_int_value(config, "TIEMPO_SUSPENSION");
    
    // Inicializar estructuras del planificador
    inicializar_planificador();
    
    // Inicializar conexiones
    inicializar_conexiones();
}

// Log de cambio de estado
void log_cambio_estado(t_pcb* pcb, t_estado_proceso estado_anterior) {
    char* nombres_estados[] = {"NEW", "READY", "EXEC", "BLOCKED", "EXIT"};
    
    log_info(logger, "## (%d) Pasa del estado %s al estado %s", 
             pcb->pid, 
             nombres_estados[estado_anterior], 
             nombres_estados[pcb->estado]);
}

// Función para cambiar el estado de un proceso
void cambiar_estado_proceso(t_pcb* pcb, t_estado_proceso nuevo_estado) {
    t_estado_proceso estado_anterior = pcb->estado;
    
    // Calcular tiempo en el estado anterior
    struct timespec ahora;
    clock_gettime(CLOCK_MONOTONIC, &ahora);
    long tiempo_en_estado = (ahora.tv_sec - pcb->inicio_estado.tv_sec) * 1000 + 
                           (ahora.tv_nsec - pcb->inicio_estado.tv_nsec) / 1000000;
    
    pcb->tiempo_estados[estado_anterior] += tiempo_en_estado;
    
    // Actualizar estado
    pcb->estado = nuevo_estado;
    pcb->contador_estados[nuevo_estado]++;
    clock_gettime(CLOCK_MONOTONIC, &pcb->inicio_estado);
    
    log_cambio_estado(pcb, estado_anterior);
}

// Planificador de Largo Plazo (FIFO)
void* planificador_largo_plazo(void* args) {
    log_info(logger, "Planificador de Largo Plazo iniciado");
    
    while (1) {
        // Esperar a que haya procesos en NEW
        sem_wait(&sem_procesos_new);
        
        if (!planificador_activo) {
            // Si llega un proceso pero el planificador no está activo, seguimos esperando
            continue;
        }
        
        // Tomar el primer proceso de la cola NEW (FIFO)
        pthread_mutex_lock(&mutex_new);
        t_pcb* pcb = queue_peek(cola_new);
        pthread_mutex_unlock(&mutex_new);
        
        // Solicitar memoria para el proceso
        bool memoria_asignada = solicitar_memoria_para_proceso(pcb);

        

        if (memoria_asignada) {
            // Quitar el proceso de la cola NEW
            pthread_mutex_lock(&mutex_new);
            queue_pop(cola_new);
            pthread_mutex_unlock(&mutex_new);
            
            // Cambiar estado a READY
            cambiar_estado_proceso(pcb, READY);
            
            // Agregar a la lista READY
            pthread_mutex_lock(&mutex_ready);
            list_add(lista_ready, pcb);
            pthread_mutex_unlock(&mutex_ready);
            
            // Señalizar que hay un proceso en READY
            sem_post(&sem_procesos_ready);
            
            log_info(logger, "## Proceso %d pasó de NEW a READY", pcb->pid);
        } else {
            // Si no hay memoria disponible, dejar el proceso en NEW
            log_info(logger, "## No hay memoria disponible para el proceso %d", pcb->pid);
            // En una implementación real, debería esperar a que se libere memoria
        }
        
    }
    
    return NULL;
}

// Log de creación de proceso
void log_creacion_proceso(t_pcb* pcb) {
    log_info(logger, "## (%d) Se crea el proceso - Estado: NEW", pcb->pid);
}

// Función para crear un nuevo PCB
t_pcb* crear_pcb(char* nombre_archivo, int tamanio) {
    t_pcb* nuevo_pcb = malloc(sizeof(t_pcb));
    
    nuevo_pcb->pid = pid_contador++;
    nuevo_pcb->pc = 0;
    nuevo_pcb->nombre_archivo = strdup(nombre_archivo);
    nuevo_pcb->tamanio = tamanio;
    nuevo_pcb->estado = NEW;
    
    // Inicializar contadores de estado
    for (int i = 0; i < 5; i++) {
        nuevo_pcb->contador_estados[i] = 0;
        nuevo_pcb->tiempo_estados[i] = 0;
    }
    
    // Incrementar contador del estado inicial (NEW)
    nuevo_pcb->contador_estados[NEW]++;
    clock_gettime(CLOCK_MONOTONIC, &nuevo_pcb->inicio_estado);
    
    log_creacion_proceso(nuevo_pcb);
    
    return nuevo_pcb;
}

// Función para agregar un nuevo proceso a la cola NEW
void agregar_proceso_new(char* nombre_archivo, int tamanio) {
    // Crear el PCB para el nuevo proceso
    t_pcb* nuevo_pcb = crear_pcb(nombre_archivo, tamanio);
    
    // Agregar a la cola NEW
    pthread_mutex_lock(&mutex_new);
    queue_push(cola_new, nuevo_pcb);
    pthread_mutex_unlock(&mutex_new);
    
    // Señalizar al planificador de largo plazo
    sem_post(&sem_procesos_new);
    
    log_info(logger, "Proceso agregado a la cola NEW - PID: %d, Archivo: %s, Tamaño: %d", nuevo_pcb->pid, nuevo_pcb->nombre_archivo, nuevo_pcb->tamanio);
}

// Destrucción de estructuras
void destruir_planificador() {
    // Liberar colas y listas
    queue_destroy(cola_new);
    list_destroy(lista_ready);
    list_destroy(lista_exec);
    list_destroy(lista_blocked);
    list_destroy(lista_exit);
    
    // Destruir mutex
    pthread_mutex_destroy(&mutex_new);
    pthread_mutex_destroy(&mutex_ready);
    pthread_mutex_destroy(&mutex_exec);
    pthread_mutex_destroy(&mutex_blocked);
    pthread_mutex_destroy(&mutex_exit);
    
    // Destruir semáforos
    sem_destroy(&sem_procesos_new);
    sem_destroy(&sem_procesos_ready);
}

// Finalizar kernel
void finalizar_kernel() {
    // Liberar memoria
    destruir_planificador();
    
    // Liberar configuración
    free(ip_memoria);
    free(algoritmo_corto_plazo);
    free(algoritmo_ingreso_a_ready);
    config_destroy(config);
    
    // Cerrar logger
    log_destroy(logger);
}

int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Uso: %s [archivo_pseudocodigo] [tamanio_proceso]\n", argv[0]);
        return 1;
    }   

    // Inicializar kernel
    inicializar_kernel();
    
    // Crear hilos para los planificadores
    pthread_t hilo_largo_plazo;
    pthread_create(&hilo_largo_plazo, NULL, planificador_largo_plazo, NULL);
    

    // Crear proceso inicial
    char* archivo_inicial = argv[1];
    int tamanio_inicial = atoi(argv[2]);
    agregar_proceso_new(archivo_inicial, tamanio_inicial);

    pthread_join(hilo_largo_plazo, NULL);

    finalizar_kernel();

    return 0;
}
