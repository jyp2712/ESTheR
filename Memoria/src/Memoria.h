/*
 * Memoria.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "protocol.h"

typedef struct{
	char* puerto;
	int marcos;
	int marco_size;
	int entradas_cache;
	int cache_x_proc;
	char* reemplazo_cache;
	int retardo_memoria;
}t_memoria;

void leerConfiguracion(t_memoria* config, char* path);

void inicializar(t_memoria* config);

void crearServidor(t_memoria* config, char *shm);

int validarHandshake(socket_t sockfd, header_t *header);

void procesarMensaje(socket_t sockfd);

void interpreteDeComandos(t_memoria* config, char *shm);

#endif /* MEMORIA_H_ */
