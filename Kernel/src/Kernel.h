/*
 * Kernel.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_
#include <semaphore.h>
#include "socket.h"
#include "structures.h"
#include <parser/parser.h>
#include <parser/metadata_program.h>

#define PACKAGESIZE 1024
#define CANT_SEM 3

enum {NEW, READY, EXEC, BLOCK, EXIT};

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
	sem_t sem_ids[CANT_SEM];
	int sem_init[CANT_SEM];
	int stack_size;
}t_kernel;

void leerConfiguracionKernel(t_kernel*, char* path);

t_pcb* crear_pcb_proceso (t_metadata_program*);

void gestion_datos_pcb (char*, socket_t, socket_t, t_list*);

#endif /* KERNEL_H_ */
