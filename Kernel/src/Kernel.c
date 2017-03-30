#include <commons/collections/list.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct t_kernel{
	int puerto_prog;
	int puerto_cpu;
	char* ip_memoria;
	int puerto_memoria;
	char* ip_fs;
	int puerto_fs;
	int quantum;
	int quantum_sleep;
	char* algoritmo;
	int grado_multiprog;
	//Faltan semaforos
	int stack_size;
}t_kernel;

int main(){

	t_config* config = config_create("./metadata");

	t_kernel* kernel = (t_kernel*) malloc(sizeof(t_kernel));

	kernel->puerto_prog = config_get_int_value(config, "PUERTO_PROG");
	kernel->puerto_cpu = config_get_int_value(config, "PUERTO_CPU");
	kernel->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	kernel->ip_fs = config_get_string_value(config, "IP_FS");
	kernel->puerto_fs = config_get_int_value(config, "PUERTO_FS");
	kernel->quantum = config_get_int_value(config, "QUANTUM");
	kernel->quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
	kernel->algoritmo = config_get_string_value(config, "ALGORITMO");
	kernel->grado_multiprog = config_get_int_value(config, "GRADO_MULTIPROG");
	kernel->stack_size = config_get_int_value(config, "STACK_SIZE");

	printf("---------------Mi configuracion---------------\n");
	printf("PUERTO_PROG: %i\n", kernel->puerto_prog);
	printf("PUNTO_CPU: %i\n", kernel->puerto_cpu);
	printf("IP_MEMORIA: %s\n", kernel->ip_memoria);
	printf("IP_FS: %s\n", kernel->ip_fs);
	printf("PUERTO_FS: %i\n", kernel->puerto_fs);
	printf("QUANTUM: %i\n", kernel->quantum);
	printf("QUANTUM_SLEEP: %i\n", kernel->quantum_sleep);
	printf("ALGORITMO: %s\n", kernel->algoritmo);
	printf("GRADO_MULTIPROG: %i\n", kernel->grado_multiprog);
	printf("STACK_SIZE: %i\n", kernel->stack_size);
	printf("----------------------------------------------\n");

	return 0;
}
