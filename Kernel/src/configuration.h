#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "Kernel.h"

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
    int stack_size;
    int page_size;
}t_kernel;

t_kernel* config;
int* sem_ansisop;
int* shared_values;
t_list** solicitudes_sem;

t_kernel *get_config(const char* path);

#endif /* CONFIGURATION_H_ */
