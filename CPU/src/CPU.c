#include "CPU.h"

int main(int argc, char **argv) {

	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CPU);

	cpu = alloc(sizeof(t_cpu));
	leerConfiguracionCPU(cpu, argv[1]);

	conectarAMemoriaYRecibirTamPag();

	conectarAKernelYRecibirStackSize();

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

void conectarAMemoriaYRecibirTamPag(){
	puts("Conectándose a la Memoria...");
	memoria_fd = socket_connect(cpu->ip_memoria, cpu->puerto_memoria);
	protocol_handshake_send(memoria_fd);
	puts("Conectado.");

	//Pedir tamPag a memoria
	log_inform("Pido tamanio de pagina a memoria");

	header_t header = protocol_header (OP_CPU_TAMPAG_REQUEST);
	packet_t packet1 = protocol_packet (header);
	protocol_packet_send(packet1, memoria_fd);

	//Recibir tamPag de memoria

	packet_t packet2 = protocol_packet_receive(memoria_fd);
	serial_unpack(packet2.payload, "h", &tamanioPagina);

	printf("Tamanio pagina obtenida: %i\n", tamanioPagina);
	log_inform("Tamanio pagina obtenida: %i", tamanioPagina);
}

void conectarAKernelYRecibirStackSize(){
	puts("Conectándose al Kernel...");
	kernel_fd = socket_connect(cpu->ip_kernel, cpu->puerto_kernel);
	protocol_handshake_send(kernel_fd);
	puts("Conectado.");

	packet_t packet = protocol_packet_receive(kernel_fd);
	serial_unpack(packet.payload, "h", &stackSize);
	log_inform("Tamaño stack recibido: %i", stackSize);
	printf("Tamaño stack recibido: %i\n", stackSize);

}

void finalizarCPU(){
	free(cpu);
	socket_close(kernel_fd);
	socket_close(memoria_fd);
}

int recibirMensajesDeKernel(){
	packet_t packet = protocol_packet_receive(kernel_fd);
	if(packet.header.opcode == OP_KE_SENDINGDATA){
		pcbActual = alloc(sizeof(t_pcb));
		serial_unpack(packet.payload, "hh", &pcbActual->idProcess, &pcbActual->pagesCode);
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

	char* proximaInstruccion = pedirProximaInstruccionAMemoria();

	log_inform("Instrucción recibida: %s", proximaInstruccion);

	analizadorLinea(proximaInstruccion, &funcionesAnSISOP, &funcionesKernel);
}

char* pedirProximaInstruccionAMemoria(){

	t_intructions *index = pcbActual->indexCode;
	index += pcbActual->PC;
	t_intructions *instruccion = index;
	int comienzo = instruccion->start;
	int longitud = instruccion->offset;

	// Obtengo la dirección lógica de la instrucción a partir del índice de código:
	t_solicitudLectura* direccionInstruccion = (sizeof(t_solicitudLectura));
	direccionInstruccion->page = comienzo / tamanioPagina;
	direccionInstruccion->offset = comienzo % tamanioPagina;
	direccionInstruccion->size = longitud;

	log_inform("Solicitando Instrucción -> Pagina: %d - Offset: %d - Size: %d.", direccionInstruccion->page, direccionInstruccion->offset, direccionInstruccion->size);

	unsigned char buff[BUFFER_CAPACITY];
	header_t header = protocol_header (OP_CPU_PROX_INST_REQUEST);
	header.msgsize = serial_pack (buff, "hhh", direccionInstruccion->page, direccionInstruccion->offset, direccionInstruccion->size);
	packet_t packet = protocol_packet (header, buff);
	protocol_packet_send(packet, memoria_fd);

	free(direccionInstruccion);

	//Recibo proxima instruccion a ejecutar
	packet_t packet2 = protocol_packet_receive(memoria_fd);

	if(packet2.header.opcode == OP_ME_PROX_INST_REQUEST_OK){

		serial_unpack(packet2.payload , "s", &proximaInstruccion);
		printf("Instruccion obtenida: %s\n", proximaInstruccion);
		return proximaInstruccion;
	}
	else if(packet2.header.opcode == OP_ME_PROX_INST_REQUEST_FAIL){

		printf("Pedido de instruccion a memoria fallo\n");
		return NULL;
	}
	return NULL;
}

// Primitivas AnSISOP

t_stack* t_stack_create(){
	t_stack* stack = alloc(sizeof(t_stack));
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
		t_var* argumento = alloc(sizeof(t_var));
		argumento->id = identificador_variable;
		argumento->mempos.offset = stackPointer;
		argumento->mempos.page = pageStack;
		argumento->mempos.size = sizeof(int);

		list_add(stack->args, argumento);

	} else {
		t_var* variable = alloc(sizeof(t_var));
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

