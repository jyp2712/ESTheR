#include "Memoria.h"
#include <pthread.h>
#include <semaphore.h>
#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"

int main(int argc, char **argv) {
	set_current_process(MEMORY);

	guard(argc == 2, "Falta indicar ruta de archivo de configuración");

	t_memoria* config = malloc(sizeof(t_memoria));
	leerConfiguracion(config, argv[1]);

	//inicializar memoria
	inicializar(config);

	//crear servidor
	crearServidor(config);

	//interprete de comandos
	interpreteDeComandos(config);

	free(config);

	return 0;
}

void leerConfiguracion(t_memoria* config, char* path){
	t_config* c = config_create(path);

	config->puerto = config_get_string_value(c, "PUERTO");
	config->marcos = config_get_int_value(c, "MARCOS");
	config->marco_size = config_get_int_value(c, "MARCO_SIZE");
	config->entradas_cache = config_get_int_value(c, "ENTRADAS_CACHE");
	config->cache_x_proc = config_get_int_value(c, "CACHE_X_PROC");
	config->reemplazo_cache = config_get_string_value(c, "REEMPLAZO_CACHE");
	config->retardo_memoria = config_get_int_value(c, "RETARDO_MEMORIA");

	printf("---------------Mi configuración---------------\n");
	printf("PUERTO: %s\n", config->puerto);
	printf("MARCOS: %i\n", config->marcos);
	printf("MARCO_SIZE: %i\n", config->marco_size);
	printf("ENTRADAS_CACHE: %i\n", config->entradas_cache);
	printf("CACHE_X_PROC: %i\n", config->cache_x_proc);
	printf("REEMPLAZO_CACHE: %s\n", config->reemplazo_cache);
	printf("RETARDO_MEMORIA: %i\n", config->retardo_memoria);
	printf("----------------------------------------------\n");
}

void inicializar(t_memoria* config){

}

void crearServidor(t_memoria* config){
	socket_t kernel_fd = socket_listen(config->puerto);

	header_t header = protocol_header_receive(kernel_fd);
	guard(header.opcode == OP_HANDSHAKE && header.syspid == KERNEL, "Unexpected handshake");
	puts("Recibido handshake del Kernel");

	char message[SOCKET_BUFFER_CAPACITY];

	while(socket_receive_string(message, kernel_fd) > 0) {
		printf("Recibido mensaje: \"%s\"\n", message);
	}

	socket_close(kernel_fd);
}

void interpreteDeComandos(t_memoria* config){

}
