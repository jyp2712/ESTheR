#include "CPU.h"
#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "serial.h"
#include "log.h"
#include <commons/collections/list.h>
#include <ctype.h>

int main(int argc, char **argv) {

	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CPU);

	cpu = malloc(sizeof(t_cpu));
	leerConfiguracionCPU(cpu, argv[1]);

	puts("Conectándose a la Memoria...");
	memoria_fd = socket_connect(cpu->ip_memoria, cpu->puerto_memoria);
	protocol_handshake_send(memoria_fd);
	puts("Conectado.");

	pedirTamPagAMemoria();

	puts("Conectándose al Kernel...");
	kernel_fd = socket_connect(cpu->ip_kernel, cpu->puerto_kernel);
	protocol_handshake_send(kernel_fd);
	puts("Conectado.");

	while(true){
		log_inform("Esperando nuevo proceso a ejecutar.");
		if (recibirMensajesDeKernel() == true) {
		} else {
			finalizarCPU();
			return EXIT_SUCCESS;
		}
	}

	finalizarCPU();
	return EXIT_SUCCESS;

}

void finalizarCPU(){
	free(cpu);
	socket_close(kernel_fd);
	socket_close(memoria_fd);
}

void pedirTamPagAMemoria(){

	//Pedir tamPag a memoria y setearlo en var local

	tamanioPagina = 10;
}

int recibirMensajesDeKernel(){

	packet_t packet = protocol_packet_receive(kernel_fd);

	if(packet.header.opcode == OP_KE_SENDINGDATA){

		pcbActual = alloc(sizeof(t_pcb));
		serial_unpack(packet.payload, "hh", &pcbActual->idProcess, &pcbActual->pagesCode);

		//Asignar pcb recibido a pcbActual

		ejecutarPrograma();

		return true;

	}
	else{
		log_report("Mensaje inválido de Kernel.");
		return false;
	}
}

void ejecutarPrograma(){

	log_inform("El proceso #%d entró en ejecución.", pcbActual->idProcess);
	int quantum = pcbActual->quantum;

	while(quantum > 0){

		char* proximaInstruccion = pedirProximaInstruccionAMemoria();

		log_inform("Instrucción recibida: %s", proximaInstruccion);

		analizadorLinea(proximaInstruccion, &funcionesAnSISOP, &funcionesKernel);

	}
}

char* pedirProximaInstruccionAMemoria(){

	return "variables a, b";


}

// Primitivas AnSISOP

t_stack* t_stack_create(){
	t_stack* stack = malloc(sizeof(t_stack));
	stack->args = list_create();
	stack->vars = list_create();
	stack->retPos = 0;
	stack->retVar.offset = 0;
	stack->retVar.page = 0;
	stack->retVar.size = 0;

	return stack;
}

bool esArgumento(t_nombre_variable identificador_variable){
	if(isdigit(identificador_variable)){
		return true;
	}else{
		return false;
	}
}

t_puntero definirVariable(t_nombre_variable identificador_variable){

	if(esArgumento(identificador_variable)){
		log_inform("Defino nuevo argumento '%c'", identificador_variable);
	} else {
		log_inform("Defino nueva variable '%c'", identificador_variable);
	}

	//Agarro el ultimo stack
	t_stack* stack = list_get(pcbActual->indexStack, pcbActual->indexStack->elements_count -1);

	if(stack == NULL){ // si no hay registros, creo uno nuevo
		stack = t_stack_create();
		// Guardo el nuevo registro en el índice:
		list_add(pcbActual->indexStack, stack);
	}

	int pageStack = 0;
	int stackPointer = pcbActual->stackPointer;

	while(stackPointer >= tamanioPagina){
		(pageStack)++;
		stackPointer -= tamanioPagina;
	}

	if(esArgumento(identificador_variable)){
		t_var* argumento = malloc(sizeof(t_var));
		argumento->id = identificador_variable;
		argumento->mempos.offset = stackPointer;
		argumento->mempos.page = pageStack;
		argumento->mempos.size = sizeof(int);

		list_add(stack->args, argumento);

	} else {
		t_var* variable = malloc(sizeof(t_var));
		variable->id = identificador_variable;
		variable->mempos.offset = stackPointer;
		variable->mempos.page = pageStack;
		variable->mempos.size = sizeof(int);

		list_add(stack->vars, variable);
	}

	log_inform("'%c' -> Dirección stack definida: %i, %i, %i.", identificador_variable, pageStack, stackPointer, sizeof(int));

	pcbActual->stackPointer += sizeof(int);

	t_puntero posicion = (pageStack * tamanioPagina) + stackPointer;
	log_inform("Posicion de '%c' es %i", identificador_variable, posicion);
	return posicion;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){

	log_inform("Obtengo posicion de '%c'", identificador_variable);

	t_stack* stack = list_get(pcbActual->indexStack, pcbActual->indexStack->elements_count -1);

	if(esArgumento(identificador_variable)){
		if(stack->args->elements_count > 0){
			int i;
			for(i = 0; i < stack->args->elements_count; i++){
				t_var* argumento = list_get(stack->args, i);
				if(argumento->id == identificador_variable){

					t_puntero posicion = (argumento->mempos.page * tamanioPagina) + argumento->mempos.offset;
					log_inform("Posicion obtenida: %i", posicion);

					return posicion;
				}
			}
		}
	} else {
		if(stack->vars->elements_count > 0){
			int i;
			for(i = 0; i < stack->vars->elements_count; i++){
				t_var* variable = list_get(stack->vars, i);
				if(variable->id == identificador_variable){

					t_puntero posicion = (variable->mempos.page * tamanioPagina) + variable->mempos.offset;
					log_inform("Posicion obtenida: %i", posicion);

					return posicion;
				}
			}
		}
	}
	return 1;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){

	t_valor_variable valorVariable;

	log_inform("Dereferenciar");

	return valorVariable;
}

//Inserta una copia del valor en la variable ubicada en direccion_variable.
void asignar(t_puntero direccion_variable, t_valor_variable valor){

	log_inform("Asignar valor");
}


void llamarSinRetorno(t_nombre_etiqueta etiqueta){

	log_inform("Llamar sin retorno");

}

void finalizar(void){

	log_inform("Finalizar");

}

void leerConfiguracionCPU(t_cpu* cpu, char* path) {

	t_config* config = config_create(path);

	cpu->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	cpu->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
	cpu->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	cpu->puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");

	printf("---------------Mi configuración---------------\n");
	printf("IP KERNEL: %s\n", cpu->ip_kernel);
	printf("PUERTO KERNEL: %s\n", cpu->puerto_kernel);
	printf("IP MEMORIA: %s\n", cpu->ip_memoria);
	printf("PUERTO MEMORIA: %s\n", cpu->puerto_memoria);
	printf("----------------------------------------------\n");
}

