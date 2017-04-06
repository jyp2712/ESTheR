/*
 * CPU.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

#define PACKAGESIZE 1024
#define BACKLOG 5

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
}t_cpu;

void leerConfiguracionCPU(t_cpu* cpu, char* path);

#endif /* CPU_H_ */
