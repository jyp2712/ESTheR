/*
 * Kernel.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_
#include <semaphore.h>
#include <parser/sintax.h>
#include <commons/collections/list.h>

#define PACKAGESIZE 1024
#define CANT_SEM 3

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

typedef struct{
	int offset;
	int size;
}t_indexCode;

typedef struct{
	int program;
	int PC;
}t_programTag;

typedef struct{
	char* id;
	int page;
	t_indexCode mempos;
}t_var;

typedef struct{
	int page;
	t_indexCode mempos;
}t_varRet;

typedef struct{
	int pos;
	t_var args;
	t_var vars;
	int retPos;
	t_varRet retVar;
}t_stack;

typedef struct {
	int idProcess;
	int PC;
	int status;
	int priority;
	int pagesCode;
	t_indexCode indexCode [PROGRAM_LINES];
	t_programTag indexTag;
	t_list* indexStack;
	int exitCode;
} t_pcb;

void leerConfiguracionKernel(t_kernel*, char* path);

#endif /* KERNEL_H_ */
