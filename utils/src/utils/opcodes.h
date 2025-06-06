#ifndef OPCODES_H_
#define OPCODES_H_

typedef enum {
    SUSPENDER_PROCESO = 2,
    FINALIZAR_PROCESO = 3,
    OBTENER_TABLA_PAGINAS = 4,
    LEER_MEMORIA = 5,
    ESCRIBIR_MEMORIA,
    LEER_PAGINA_COMPLETA,
    ACTUALIZAR_PAGINA_COMPLETA,
    MEMORY_DUMP,
    RESPUESTA_OK,
    RESPUESTA_ERROR,
    TEST_COMUNICACIONAL = 1,
} op_code;

#endif