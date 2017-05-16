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

typedef int t_page_detail[2]; //pid, nro paging
typedef t_page_detail t_pages[];

t_memoria *get_config(const char* path);

#endif
