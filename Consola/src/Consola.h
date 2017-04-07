/*
 * Consola.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
} t_consola;

void leerConfiguracionConsola(t_consola *consola, char *path);


#endif /* CONSOLA_H_ */
