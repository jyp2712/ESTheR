#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include "Memoria.h"
#include "../../Common/sockets.h"

int main(int argc, char **argv){

	if(argc == 1){
		printf("Falta Indicar ruta de archivo de configuracion\n");
		abort();
	}

	char* path = argv[1];

	t_memoria* memoria = (t_memoria*) malloc(sizeof(t_memoria));

	leerConfiguracionMemoria(memoria, path);

	free(memoria);

	return 0;

}

void leerConfiguracionMemoria(t_memoria* memoria, char* path){

	t_config* config = config_create(path);

	memoria->puerto = config_get_string_value(config, "PUERTO");
	memoria->marcos = config_get_int_value(config, "MARCOS");
	memoria->marco_size = config_get_int_value(config, "MARCO_SIZE");
	memoria->entradas_cache = config_get_int_value(config, "ENTRADAS_CACHE");
	memoria->cache_x_proc = config_get_int_value(config, "CACHE_X_PROC");
	memoria->reemplazo_cache = config_get_string_value(config, "REEMPLAZO_CACHE");
	memoria->retardo_memoria = config_get_int_value(config, "RETARDO_MEMORIA");

	printf("---------------Mi configuracion---------------\n");
	printf("PUERTO: %s\n", memoria->puerto);
	printf("MARCOS: %i\n", memoria->marcos);
	printf("MARCO_SIZE: %i\n", memoria->marco_size);
	printf("ENTRADAS_CACHE: %i\n", memoria->entradas_cache);
	printf("CACHE_X_PROC: %i\n", memoria->cache_x_proc);
	printf("REEMPLAZO_CACHE: %s\n", memoria->reemplazo_cache);
	printf("RETARDO_MEMORIA: %i\n", memoria->retardo_memoria);
	printf("----------------------------------------------\n");
}
