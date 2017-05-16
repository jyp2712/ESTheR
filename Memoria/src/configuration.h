#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "Memoria.h"
#include <commons/config.h>

typedef struct{
	int puerto;
	int marcos;
	int marco_size;
	int entradas_cache;
	int cache_x_proc;
	int retardo_memoria;
} t_memoria;

t_memoria *get_config(const char* path);

#endif
