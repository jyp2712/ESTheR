#include <commons/config.h>
#include <commons/string.h>
#include "utils.h"
#include "socket.h"
#include "Kernel.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>
#include <semaphore.h>


int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");

	t_log* LogKernel = crearArchivoLog("Kernel");

	t_kernel* kernel = malloc(sizeof(t_kernel));
	leerConfiguracionKernel(kernel, argv[1]);
	log_info(LogKernel, "Lee configuracion del proceso");
	
	printf("Conectandose al servidor...\n");
	int memoria_fd = socket_connect(kernel->ip_memoria, kernel->puerto_memoria);
	int fs_fd = socket_connect(kernel->ip_fs, kernel->puerto_fs);
	printf("Conectado al servidor. Ya puede enviar mensajes.\n");

	socket_select(kernel->puerto_prog, memoria_fd, fs_fd);

	free(kernel);
	return 0;
}

void leerConfiguracionKernel(t_kernel* kernel, char* path){

		t_config* config = config_create(path);

		kernel->puerto_prog = config_get_string_value(config, "PUERTO_PROG");
		kernel->puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
		kernel->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
		kernel->puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
		kernel->ip_fs = config_get_string_value(config, "IP_FS");
		kernel->puerto_fs = config_get_string_value(config, "PUERTO_FS");
		kernel->quantum = config_get_int_value(config, "QUANTUM");
		kernel->quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
		kernel->algoritmo = config_get_string_value(config, "ALGORITMO");
		kernel->grado_multiprog = config_get_int_value(config, "GRADO_MULTIPROG");
		char** semaforos = config_get_array_value(config, "SEM_IDS");
		for(int i=0; i<3; i++){
			strcpy (kernel->sem_ids[i].__size, semaforos[i]);
		}
		char** semaforos_init = config_get_array_value(config, "SEM_INIT");
		for(int i=0; i<3; i++){
			kernel->sem_init[i] = atoi (semaforos_init[i]);
		}
		kernel->stack_size = config_get_int_value(config, "STACK_SIZE");

		printf("---------------Mi configuración---------------\n");
		printf("PUERTO_PROG: %s\n", kernel->puerto_prog);
		printf("PUERTO_CPU: %s\n", kernel->puerto_cpu);
		printf("IP_MEMORIA: %s\n", kernel->ip_memoria);
		printf("PUERTO_MEMORIA: %s\n", kernel->puerto_memoria);
		printf("IP_FS: %s\n", kernel->ip_fs);
		printf("PUERTO_FS: %s\n", kernel->puerto_fs);
		printf("QUANTUM: %i\n", kernel->quantum);
		printf("QUANTUM_SLEEP: %i\n", kernel->quantum_sleep);
		printf("ALGORITMO: %s\n", kernel->algoritmo);
		printf("SEM_IDS:[%s, %s ,%s]\n", kernel->sem_ids[0].__size, kernel->sem_ids[1].__size, kernel->sem_ids[2].__size);
		printf("SEM_INIT:[%d, %d ,%d]\n", kernel->sem_init[0], kernel->sem_init[1], kernel->sem_init[2]);
		printf("STACK_SIZE: %i\n", kernel->stack_size);
		printf("----------------------------------------------\n");
}
