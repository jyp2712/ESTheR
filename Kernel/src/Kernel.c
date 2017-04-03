#include <commons/collections/list.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>


/* ******************************************************************
 *                     	  TIPOS DE DATOS
 * *****************************************************************/

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


/* ******************************************************************
 *                     	   PROTOTIPOS DE
 *                           FUNCIONES
 * *****************************************************************/

void leerParametrosConfiguracion (t_config*, t_kernel*);

void crearConexion ();


/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/

int main(){

	t_config* config = config_create("./metadata");
	t_kernel* kernel = (t_kernel*) malloc(sizeof(t_kernel));

	leerParametrosConfiguracion (config, kernel);
	crearConexion ();

	return 0;
}

/* ******************************************************************
 *                     		FUNCIONES
 * *****************************************************************/

void leerParametrosConfiguracion (t_config* config, t_kernel* kernel){

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

}

void crearConexion (){

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(8080);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Falló el bind");
		return;
	}

	listen(servidor, 100);

	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion;
	int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

	char* buffer = malloc(1000);

	while (1) {
		int bytesRecibidos = recv(cliente, buffer, 1000, 0);
		if (bytesRecibidos <= 0) {
			perror("Cliente Desconectado");
			return;
		}

		buffer[bytesRecibidos] = '\0';
		printf("%s\n", buffer);
	}

	free(buffer);
}
