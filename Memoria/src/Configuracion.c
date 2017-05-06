/*
 * Config.c
 *
 *  Created on: 24/4/2017
 *      Author: utnso
 */
#include <commons/config.h>
#include "Configuracion.h"

void leerConfiguracion(t_memoria* config, char* path){
	t_config* c = config_create(path);

	config->puerto = config_get_string_value(c, "PUERTO");
	config->marcos = config_get_int_value(c, "MARCOS");
	config->marco_size = config_get_int_value(c, "MARCO_SIZE");
	config->entradas_cache = config_get_int_value(c, "ENTRADAS_CACHE");
	config->cache_x_proc = config_get_int_value(c, "CACHE_X_PROC");
	config->retardo_memoria = config_get_int_value(c, "RETARDO_MEMORIA");

	title("Configuración");
	printf("PUERTO: %s\n", config->puerto);
	printf("MARCOS: %i\n", config->marcos);
	printf("MARCO_SIZE: %i\n", config->marco_size);
	printf("ENTRADAS_CACHE: %i\n", config->entradas_cache);
	printf("CACHE_X_PROC: %i\n", config->cache_x_proc);
	printf("RETARDO_MEMORIA: %i\n", config->retardo_memoria);
}
