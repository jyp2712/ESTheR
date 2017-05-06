/*
 * Memoria.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "socket.h"

void inicializar(void);

void crearServidor(void);

void procesarCliente(void *arg);

void quitarConexion(socket_t sockfd, char *msg);

void interpreteDeComandos(void);

char* buscarProximaInstruccion();

void terminate(void);

#endif /* MEMORIA_H_ */
