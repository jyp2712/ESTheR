/*
 * Config.h
 *
 *  Created on: 24/4/2017
 *      Author: utnso
 */

#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include "Memoria.h"
#include <commons/config.h>

typedef struct{
	char* puerto;
	int marcos;
	int marco_size;
	int entradas_cache;
	int cache_x_proc;
	int retardo_memoria;
}t_memoria;

void leerConfiguracion(t_memoria* config, char* path);

#endif /* CONFIGURACION_H_ */
