#include "configuration.h"

t_kernel *get_config(const char *path) {
	t_config* c = config_create((char *)path);
	t_kernel *config = malloc(sizeof(t_kernel));

	config->puerto_prog = config_get_string_value(c, "PUERTO_PROG");
	config->puerto_cpu = config_get_string_value(c, "PUERTO_CPU");
	config->ip_memoria = config_get_string_value(c, "IP_MEMORIA");
    config->puerto_memoria = config_get_string_value(c, "PUERTO_MEMORIA");
    config->ip_fs = config_get_string_value(c, "IP_FS");
    config->puerto_fs = config_get_string_value(c, "PUERTO_FS");
    config->quantum = config_get_int_value(c, "QUANTUM");
    config->quantum_sleep = config_get_int_value(c, "QUANTUM_SLEEP");
    config->algoritmo = config_get_string_value(c, "ALGORITMO");
    config->grado_multiprog = config_get_int_value(c, "GRADO_MULTIPROG");
    config->sem_ids = config_get_array_value(c, "SEM_IDS");
    config->sem_init = config_get_array_value(c, "SEM_INIT");
    int counter = 0;
    while (config->sem_ids[counter]) counter++;
    int initValue;
    config->sem_ansisop = malloc(counter * sizeof(sem_t));
    config->solicitudes_sem = malloc(counter * sizeof(t_list));
    for(int i=0; i<counter; i++){
    	config->solicitudes_sem[i] = list_create();
    	initValue = atoi(config->sem_init[i]);
    	sem_init(&config->sem_ansisop[i], 0, initValue);
    }
    config->shared_vars = config_get_array_value (c, "SHARED_VARS");
    counter = 0;
    while (config->shared_vars[counter]) counter++;
    config->shared_values = malloc(counter * sizeof(int));
    for(int i=0; i<counter; i++) config->shared_values[i] = 0;
    config->stack_size = config_get_int_value(c, "STACK_SIZE");
    //config_destroy(c); //TODO revisar la destruccion de este objeto

    title("Configuración");

    printf("PUERTO_PROG: %s\n", config->puerto_prog);
    printf("PUERTO_CPU: %s\n", config->puerto_cpu);
    printf("IP_MEMORIA: %s\n", config->ip_memoria);
    printf("PUERTO_MEMORIA: %s\n", config->puerto_memoria);
    printf("IP_FS: %s\n", config->ip_fs);
    printf("PUERTO_FS: %s\n", config->puerto_fs);
    printf("QUANTUM: %i\n", config->quantum);
    printf("QUANTUM_SLEEP: %i\n", config->quantum_sleep);
    printf("ALGORITMO: %s\n", config->algoritmo);
    printf("GRADO_MULTIPROG: %i\n", config->grado_multiprog);
    printf("SEM_IDS:[%s, %s ,%s]\n", config->sem_ids[0], config->sem_ids[1], config->sem_ids[2]);
    printf("SEM_INIT:[%s, %s ,%s]\n", config->sem_init[0], config->sem_init[1], config->sem_init[2]);
    printf("SHARED_VARS:[!%s, !%s ,!%s]\n", config->shared_vars[0], config->shared_vars[1], config->shared_vars[2]);
    printf("STACK_SIZE: %i\n", config->stack_size);

    return config;
}


