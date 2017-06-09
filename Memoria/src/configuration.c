#include <commons/config.h>
#include "configuration.h"

t_memoria *get_config(const char *path) {
	t_config* c = config_create((char *)path);
	config = malloc(sizeof(t_memoria));
	config->puerto = strtoi(config_get_string_value(c, "PUERTO"));
	config->marcos = config_get_int_value(c, "MARCOS");
	config->marco_size = config_get_int_value(c, "MARCO_SIZE");
	config->entradas_cache = config_get_int_value(c, "ENTRADAS_CACHE");
	config->cache_x_proc = config_get_int_value(c, "CACHE_X_PROC");
	config->retardo_memoria = config_get_int_value(c, "RETARDO_MEMORIA");
	config_destroy(c);

	title("Configuración");
	printf("PUERTO: %d\n", config->puerto);
	printf("MARCOS: %d\n", config->marcos);
	printf("MARCO_SIZE: %d\n", config->marco_size);
	printf("ENTRADAS_CACHE: %d\n", config->entradas_cache);
	printf("CACHE_X_PROC: %d\n", config->cache_x_proc);
	printf("RETARDO_MEMORIA: %d\n", config->retardo_memoria);

	return config;
}
