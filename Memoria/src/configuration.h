#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <commons/config.h>
#include "Memoria.h"
#include "page_table.h"

// Configuracion
typedef struct{
	int puerto;
	int marcos;
	int marco_size;
	int entradas_cache;
	int cache_x_proc;
	int retardo_memoria;
} t_memoria;

// Estado global de la Memoria
typedef struct {
	byte *main; 	  			// Puntero a la memoria principal
	unsigned delay;   			// Retardo de acceso en milisegundos
	unsigned nframes; 			// Cantidad de marcos
	size_t sframe;    			// Tamaño de cada marco
	t_page_table *page_table;	// Tabla de paginas
} t_memory_data;

t_memoria *get_config(const char* path);

#endif
