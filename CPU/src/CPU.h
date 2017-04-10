#ifndef CPU_H_
#define CPU_H_

#define PACKAGESIZE 1024
#define BACKLOG 5

#include <parser/parser.h>

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
}t_cpu;

void leerConfiguracionCPU(t_cpu* cpu, char* path);
t_puntero definirVariable(t_nombre_variable identificador_variable);
t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable);
t_valor_variable dereferenciar(t_puntero direccion_variable);
void asignar(t_puntero direccion_variable, t_valor_variable valor);
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);
void irAlLabel(t_nombre_etiqueta t_nombre_etiqueta);
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);
void retornar(t_valor_variable retorno);
void finalizar(void);
void llamarSinRetorno(t_nombre_etiqueta etiqueta);

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
};


#endif /* CPU_H_ */
