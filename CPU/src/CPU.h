#ifndef CPU_H_
#define CPU_H_

#define PACKAGESIZE 1024
#define BACKLOG 5

#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "serial.h"
#include "log.h"
#include <commons/collections/list.h>
#include <ctype.h>
#include <parser/parser.h>
#include "structures.h"

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
	char* ip_memoria;
	char* puerto_memoria;
}t_cpu;

tamanioVariable = sizeof(int);
bool procesoBloqueado = false;
bool huboStackOver = false;
bool finPrograma = false;
char proximaInstruccion[BUFFER_CAPACITY];
int tamanioPagina;
int memoria_fd;
int kernel_fd;
int stackSize;
t_pcb* pcbActual;
t_cpu* cpu;

int verificarTerminarEjecucion();
void conectarAMemoria();
void conectarAKernelYRecibirStackSizeYTamPag();
void finalizarCPU();
t_stack* t_stack_create();
bool esArgumento(t_nombre_variable identificador_variable);
char* pedirProximaInstruccionAMemoria();
void ejecutarPrograma();
void pedirTamPagAMemoria();
void recibirMensajesDeKernel();
void leerConfiguracionCPU(t_cpu* cpu, char* path);
t_puntero definirVariable(t_nombre_variable identificador_variable);
t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable);
t_valor_variable dereferenciar(t_puntero direccion_variable);
void asignar(t_puntero direccion_variable, t_valor_variable valor);
void finalizar(void);
void llamarSinRetorno(t_nombre_etiqueta etiqueta);
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);
void retornar(t_valor_variable retorno);
void irAlLabel(t_nombre_etiqueta etiqueta);
void signalAnsisop(t_nombre_semaforo identificador_semaforo);
void wait(t_nombre_semaforo identificador_semaforo);
t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);

// Estructuras funcionesAnSISOP
AnSISOP_funciones funcionesAnSISOP = {
		.AnSISOP_definirVariable			= definirVariable,
		.AnSISOP_obtenerPosicionVariable	= obtenerPosicionVariable,
		.AnSISOP_dereferenciar				= dereferenciar,
		.AnSISOP_asignar					= asignar,
		.AnSISOP_obtenerValorCompartida		= obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida		= asignarValorCompartida,
		.AnSISOP_irAlLabel					= irAlLabel,
		.AnSISOP_llamarConRetorno			= llamarConRetorno,
		.AnSISOP_llamarSinRetorno			= llamarSinRetorno,
		.AnSISOP_retornar					= retornar,
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
		.AnSISOP_signal						= signalAnsisop,
		.AnSISOP_wait						= wait,
};


#endif /* CPU_H_ */
