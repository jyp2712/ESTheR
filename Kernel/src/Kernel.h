/*
 * Kernel.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define PACKAGESIZE 1024

typedef struct{
	char* puerto_prog;
	char* puerto_cpu;
	char* ip_memoria;
	char* puerto_memoria;
	char* ip_fs;
	char* puerto_fs;
	int quantum;
	int quantum_sleep;
	char* algoritmo;
	int grado_multiprog;
	//Faltan semaforos
	int stack_size;
}t_kernel;

void leerConfiguracionKernel(t_kernel*, char* path);
void create_serverSocket(int* listenningSocket, char* port);
void accept_connection(int listenningSocket, int* clientSocket);
void create_socketClient(int* serverSocket, char* ip, char* port);

#endif /* KERNEL_H_ */
