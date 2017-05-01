#ifndef CPU_H_
#define CPU_H_

#define PACKAGESIZE 1024
#define BACKLOG 5

#include <parser/parser.h>
#include "structures.h"

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
	char* ip_memoria;
	char* puerto_memoria;
}t_cpu;

int tamanioPagina;
int memoria_fd;
int kernel_fd;
t_pcb* pcbActual;
t_cpu* cpu;

void finalizarCPU();
t_stack* t_stack_create();
bool esArgumento(t_nombre_variable identificador_variable);
char* pedirProximaInstruccionAMemoria();
void ejecutarPrograma();
void pedirTamPagAMemoria();
int recibirMensajesDeKernel();
void leerConfiguracionCPU(t_cpu* cpu, char* path);
t_puntero definirVariable(t_nombre_variable identificador_variable);
t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable);
t_valor_variable dereferenciar(t_puntero direccion_variable);
void asignar(t_puntero direccion_variable, t_valor_variable valor);
void finalizar(void);
void llamarSinRetorno(t_nombre_etiqueta etiqueta);

// Estructuras funcionesAnSISOP
AnSISOP_funciones funcionesAnSISOP = {
		.AnSISOP_definirVariable			= definirVariable,
		.AnSISOP_obtenerPosicionVariable	= obtenerPosicionVariable,
		.AnSISOP_dereferenciar				= dereferenciar,
		.AnSISOP_asignar					= asignar,
		//.AnSISOP_obtenerValorCompartida	= obtenerValorCompartida,
		//.AnSISOP_asignarValorCompartida	= asignarValorCompartida,
		//.AnSISOP_irAlLabel				= irAlLabel,
		//.AnSISOP_llamarConRetorno			= llamarConRetorno,
		.AnSISOP_llamarSinRetorno			= llamarSinRetorno,
		//.AnSISOP_retornar					= retornar,
		.AnSISOP_finalizar					= finalizar,
};

// Estructuras funciones Kernel
AnSISOP_kernel funcionesKernel = {
		//.AnSISOP_abrir					= abrir
		//.AnSISOP_borrar					= borrar
		//.AnSISOP_cerrar					= cerrar
		//.AnSISOP_escribir					= escribir
		//.AnSISOP_leer						= leer
		//.AnSISOP_liberar					= liberar
		//.AnSISOP_moverCursor				= moverCursor
		//.AnSISOP_reservar					= reservar
		//.AnSISOP_signal					= signal
		//.AnSISOP_wait						= wait
};


#endif /* CPU_H_ */
