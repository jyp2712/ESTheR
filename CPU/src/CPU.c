#include "CPU.h"

int main(int argc, char **argv) {

	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CPU);

	title("CPU");
	cpu = alloc(sizeof(t_cpu));
	leerConfiguracionCPU(cpu, argv[1]);

	title("Conexión");
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
	printf("Estableciendo conexión con la Memoria...");
	memoria_fd = socket_connect(cpu->ip_memoria, cpu->puerto_memoria);
	protocol_handshake_send(memoria_fd);
	printf("\33[2K\rConectado a la Memoria en %s:%s\n", cpu->ip_memoria, cpu->puerto_memoria);

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
	printf("Estableciendo conexión con el Kernel...");
	kernel_fd = socket_connect(cpu->ip_kernel, cpu->puerto_kernel);
	protocol_handshake_send(kernel_fd);
	printf("\33[2K\rConectado al Kernel en %s:%s\n", cpu->ip_kernel, cpu->puerto_kernel);

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
	if(packet.header.opcode == OP_KE_SEND_PCB){
		pcbActual = alloc(sizeof(t_pcb));
		serial_unpack_pcb (pcbActual, packet.payload);

		for(int i= 0; i < pcbActual->instructions; i++){
			printf("%d\n%d\n", (pcbActual->indexCode+i)->start, (pcbActual->indexCode+i)->offset);
		}

		/*ejecutarPrograma();*/
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
	t_solicitudLectura* lectura = alloc(sizeof(t_solicitudLectura));
	lectura->idProcess = pcbActual->idProcess;
	lectura->page = comienzo / tamanioPagina;
	lectura->offset = comienzo % tamanioPagina;
	lectura->size = longitud;

	log_inform("Solicitando Instrucción -> Pid: %d Pagina: %d - Offset: %d - Size: %d.", lectura->idProcess, lectura->page, lectura->offset, lectura->size);

	unsigned char buff[BUFFER_CAPACITY];
	header_t header = protocol_header (OP_CPU_PROX_INST_REQUEST);
	header.msgsize = serial_pack (buff, "hhhh", lectura->idProcess, lectura->page, lectura->offset, lectura->size);
	packet_t packet = protocol_packet (header, buff);
	protocol_packet_send(packet, memoria_fd);

	free(lectura);

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

	if(pcbActual->stackPointer + 4 > stackSize * tamanioPagina){
		if(!huboStackOver){
			printf("StackOverFlow");
			log_report("StackOverflow. Se finaliza el proceso");
			huboStackOver = true;
		}
		return -1;
	}

	if(!esArgumento(identificador_variable)){

		log_inform("Defino nuevo variable '%c'", identificador_variable);
		t_var* nuevaVar = malloc(sizeof(t_var));
		t_stack* lineaStack = list_get(pcbActual->stack, pcbActual->stack->elements_count -1);

		if(lineaStack == NULL){ // si no hay registros, creo uno nuevo
			lineaStack = t_stack_create();
			list_add(pcbActual->stack, lineaStack);
		}

		nuevaVar->id = identificador_variable;
		nuevaVar->mempos.page = pcbActual->stackPointer / tamanioPagina; //sumar la cantPaginas al hacer la solicitud + pcbActual->pagesCode;
		nuevaVar->mempos.offset = pcbActual->stackPointer % tamanioPagina;
		nuevaVar->mempos.size = sizeof(int);
		list_add(lineaStack->vars, nuevaVar);
		pcbActual->stackPointer += sizeof(int);

		log_inform("Posicion relativa de %c -> %d %d %d", nuevaVar->id, nuevaVar->mempos.page, nuevaVar->mempos.offset, nuevaVar->mempos.size);
		log_inform("Posicion absoluta de %c: %i", identificador_variable, pcbActual->stackPointer - sizeof(int));
		return pcbActual->stackPointer - sizeof(int);

	} else {

		log_inform("Defino nuevo argumento '%c'", identificador_variable);
		t_var* nuevoArg = malloc(sizeof(t_var));
		t_stack* lineaStack = list_get(pcbActual->stack, pcbActual->stack->elements_count - 1);

		if(lineaStack == NULL){ // si no hay registros, creo uno nuevo
			lineaStack = t_stack_create();
			lineaStack->retPos = pcbActual->PC; // se crea en -1
			list_add(pcbActual->stack, lineaStack);
		}

		nuevoArg->mempos.page = pcbActual->stackPointer / tamanioPagina; // + pcbActual->pagesCode
		nuevoArg->mempos.offset = pcbActual->stackPointer % tamanioPagina;
		nuevoArg->mempos.size = sizeof(int);
		/*list_add(lineaStack->args, nuevoArg);*/
		pcbActual->stackPointer += sizeof(int);

		log_inform("Posicion relativa de %c -> %d %d %d", identificador_variable, nuevoArg->mempos.page, nuevoArg->mempos.offset, nuevoArg->mempos.size);
		log_inform("Posicion absoluta de %c: %d", identificador_variable, pcbActual->stackPointer - sizeof(int));
		return pcbActual->stackPointer - sizeof(int);

	}
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){

	log_inform("Obtengo posicion de '%c'", identificador_variable);

	int i;
	t_stack* entradaStack;
	t_var* var_local;
	t_var* argumento;
	t_puntero posicion;

	if(pcbActual->stack->elements_count == 0){
		log_report("No hay nada en el indice de stack");
		return EXIT_FAILURE;
	}
	else{
		entradaStack = list_get(pcbActual->stack, pcbActual->stack->elements_count - 1);

		if(!esArgumento(identificador_variable)){ // es una variable
			for(i=0; i < entradaStack->vars->elements_count; i++){
				var_local = list_get(entradaStack->vars, i);
				if(var_local->id == identificador_variable)
					break;
			}
			if(entradaStack->vars->elements_count == i){
				log_report("No se encontro la variable %c en el stack", identificador_variable);
				return EXIT_FAILURE;
			}
			else{
				posicion = var_local->mempos.page * tamanioPagina + var_local->mempos.offset;
			}
		} // es un argumento
		else{
			if(identificador_variable > entradaStack->args->elements_count){
				return EXIT_FAILURE;
			}else{
				argumento = list_get(entradaStack->args,identificador_variable);
				posicion = argumento->mempos.page * tamanioPagina + argumento->mempos.offset;
			}
		}
	}
	log_inform("Posicion obtenida: %i", posicion);
	return posicion;

}

t_valor_variable dereferenciar(t_puntero direccion_variable){

	t_valor_variable valorVariable;

	log_inform("Dereferenciar");

	return valorVariable;
}

//Inserta una copia del valor en la variable ubicada en direccion_variable.
void asignar(t_puntero direccion_variable, t_valor_variable valor){

	log_inform("Asignar -> posicion var: %d - valor: %d", direccion_variable, valor);

}

void llamarSinRetorno(t_nombre_etiqueta etiqueta){

	log_inform("Llamar sin retorno");

}

void irAlLabel(t_nombre_etiqueta etiqueta){
		log_inform("Ir a label %s", etiqueta);
		t_puntero_instruccion numeroInstr = metadata_buscar_etiqueta(etiqueta, pcbActual->indexTag, pcbActual->tags);
		log_inform("Instruccion del irALAbel: %d", numeroInstr);
		if(numeroInstr == -1){
			log_report("No se encontro la etiqueta");
			return;
		}
		pcbActual->PC = numeroInstr - 1; //revisar despues
}

void finalizar(void){

	log_inform("ANSISOP_finalizar");
	// Obtengo contexto quitado de la lista y lo limpio.
	t_stack* contexto = list_remove(pcbActual->stack, list_size(pcbActual->stack) - 1);
	int i = list_size(contexto->args) + list_size(contexto->vars);
	pcbActual->stackPointer-= sizeof(int)*i; // Disminuyo stackPointer del pcb
	for(i = 0; i < list_size(contexto->args); i++){ // Limpio lista de argumentos del contexto
		free(list_remove(contexto->args,i));
	}
	for(i = 0; i < list_size(contexto->vars); i++){ // Limpio lista de variables del contexto
		free(list_remove(contexto->vars, i));
	}
	list_destroy(contexto->args);
	list_destroy(contexto->vars);

	if(list_size(pcbActual->stack) == 0){
		finPrograma = true;
		log_inform("Finalizó la ejecucion del programa.");
	}else{
		pcbActual->PC = contexto->retPos;
	}
	free(contexto);
	return;

}

void leerConfiguracionCPU(t_cpu* cpu, char* path) {

	t_config* config = config_create(path);

	cpu->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	cpu->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
	cpu->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	cpu->puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");

	title("Configuración");
	printf("IP KERNEL: %s\n", cpu->ip_kernel);
	printf("PUERTO KERNEL: %s\n", cpu->puerto_kernel);
	printf("IP MEMORIA: %s\n", cpu->ip_memoria);
	printf("PUERTO MEMORIA: %s\n", cpu->puerto_memoria);
}

