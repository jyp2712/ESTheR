/*
 * Kernel.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_
#include <semaphore.h>

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
	sem_t sem_ids[3];
	int sem_init[3];
	int stack_size;
}t_kernel;

typedef struct{
	int page;
	int offset;
	int size;
}t_indexCode;

typedef struct{
	int program;
	int PC;
}t_programTag;

typedef struct{
	char* id;
	t_indexCode mempos;
}t_var;

typedef struct{
	int pos;
	t_var args;
	t_var vars;
	int retPos;
	t_indexCode retVar;
}t_stack;

typedef struct {
	int idProcess;
	int PC;
	int pagesCode;
	t_indexCode* indexCode;
	t_programTag indexTag;
	t_stack indexStack;
	int exitCode;
} t_PCB;

void leerConfiguracionKernel(t_kernel*, char* path);

#endif /* KERNEL_H_ */
