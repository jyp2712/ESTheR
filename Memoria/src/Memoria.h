/*
 * Memoria.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "protocol.h"
#include <signal.h>

typedef struct{
	char* puerto;
	int marcos;
	int marco_size;
	int entradas_cache;
	int cache_x_proc;
	char* reemplazo_cache;
	int retardo_memoria;
}t_memoria;

void inicializar(void);

void crearServidor(void);

void procesarCliente(void *arg);

void quitarConexion(socket_t sockfd, char *msg);

void interpreteDeComandos(void);

void terminate(void);

#endif /* MEMORIA_H_ */
