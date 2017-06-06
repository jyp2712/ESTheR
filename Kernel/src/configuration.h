#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <commons/config.h>
#include <commons/collections/list.h>
#include <semaphore.h>

typedef struct{
    char* puerto_prog;
    char* puerto_cpu;
    char* ip_memoria;
    char* puerto_memoria;
    char* ip_fs;
    char* puerto_fs;
    int quantum;
    int quantum_sleep;
    char* algoritmo;
    int grado_multiprog;
    char** sem_ids;
    char** sem_init;
    char** shared_vars;
    int* shared_values;
    int stack_size;
    int page_size;
    sem_t *sem_ansisop;
    t_list** solicitudes_sem;
}t_kernel;

t_kernel *get_config(const char* path);

#endif /* CONFIGURATION_H_ */
