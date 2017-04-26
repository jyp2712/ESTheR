/*
 * Consola.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

//#include <globals.h>

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
} t_consola;

typedef struct{    //estructura para enviar al hilo que inicia programa
	char * buffer;
	const char * path;
	int kernel_fd;
}t_parametrosHilo;

void leerConfiguracionConsola(t_consola *consola, char *path);

void  iniciarPrograma(t_parametrosHilo * pamHilo);//Funcion para que cada hilo
										//inice un programa

#endif /* CONSOLA_H_ */
