#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "socket.h"

// Configuracion
typedef struct{
	int puerto;
	int marcos;
	int marco_size;
	int entradas_cache;
	int cache_x_proc;
	int retardo_memoria;
} t_memoria;

t_memoria* config;

unsigned memory_get_access_time();

void memory_set_access_time(unsigned delay);

unsigned memory_get_frame_number();

size_t memory_get_frame_size();

void memory_flush_cache();

#endif /* MEMORIA_H_ */
