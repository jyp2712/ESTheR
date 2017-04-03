#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
	char* puerto;
	int marcos;
	int marco_size;
	int entradas_cache;
	int cache_x_proc;
	char* reemplazo_cache;
	int retardo_memoria;
}t_memoria;

void leerConfiguracionMemoria(t_memoria* memoria);

int main(){

	t_memoria* memoria = (t_memoria*) malloc(sizeof(t_memoria));

	leerConfiguracionMemoria(memoria);

	free(memoria);

	return 0;

}

void leerConfiguracionMemoria(t_memoria* memoria){

	t_config* config = config_create("../metadata");

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
