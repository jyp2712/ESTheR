/*
 * Kernel.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

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
	char** sem_ids;
	char** sem_init;
	int stack_size;
}t_kernel;

void leerConfiguracionKernel(t_kernel*, char* path);

#endif /* KERNEL_H_ */
