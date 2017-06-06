#include "CPU.h"

int main(int argc, char **argv) {

	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CPU);

	title("CPU");
	cpu = alloc(sizeof(t_cpu));
	leerConfiguracionCPU(cpu, argv[1]);

	title("Conexión");
	conectarAMemoria();

	conectarAKernelYRecibirStackSizeYTamPag();

	while(true){
		recibirMensajesDeKernel();
	}

	finalizarCPU();
	return EXIT_SUCCESS;

}

void recibirMensajesDeKernel(){

	packet_t packet = protocol_packet_receive(kernel_fd);

	switch(packet.header.opcode){
		case OP_KE_SEND_PCB:
			pcbActual = alloc(sizeof(t_pcb));
			serial_unpack_pcb (pcbActual, packet.payload);
			ejecutarPrograma();
			break;
		case OP_KE_SEND_QUANTUM:
			ejecutarPrograma();
			break;
		case OP_CPU_PROGRAM_END:
			break;
		default:
			log_report("Mensaje Recibido Incorrecto");
		}
}

void conectarAMemoria(){
	printf("Estableciendo conexión con la Memoria...\n");
	memoria_fd = socket_connect(cpu->ip_memoria, cpu->puerto_memoria);
	protocol_handshake_send(memoria_fd);
	printf("\33[2K\rConectado a la Memoria en %s:%s\n", cpu->ip_memoria, cpu->puerto_memoria);

}

void conectarAKernelYRecibirStackSizeYTamPag(){
	printf("Estableciendo conexión con el Kernel...\n");
	kernel_fd = socket_connect(cpu->ip_kernel, cpu->puerto_kernel);
	protocol_handshake_send(kernel_fd);
	printf("\33[2K\rConectado al Kernel en %s:%s\n", cpu->ip_kernel, cpu->puerto_kernel);

	packet_t packet = protocol_packet_receive(kernel_fd);
	serial_unpack(packet.payload, "hh", &stackSize, &tamanioPagina);
	log_inform("Tamaño stack recibido: %i", stackSize);
	printf("Tamaño stack recibido: %i\n", stackSize);

	printf("Tamanio pagina obtenida: %i\n", tamanioPagina);
	log_inform("Tamanio pagina obtenida: %i", tamanioPagina);

}

void finalizarCPU(){
	free(cpu);
	socket_close(kernel_fd);
	socket_close(memoria_fd);
}

void ejecutarPrograma(){

	pcbActual->quantum = 5;

	log_inform("El proceso #%d entró en ejecución.", pcbActual->idProcess);
	if(pcbActual->quantum == 0){
		log_inform("Ejecutar - Algoritmo FIFO");
	}else{
		log_inform("Ejecutar - Algoritmo RR con Q = %d", pcbActual->quantum);
	}

	int i = 1;
	procesoBloqueado = false;

	while(i <= pcbActual->quantum || pcbActual->quantum == 0){
		char* proximaInstruccion = pedirProximaInstruccionAMemoria();
		analizadorLinea(proximaInstruccion, &funcionesAnSISOP, &funcionesKernel);
		if (verificarTerminarEjecucion() == -1) return;
		i++;
		pcbActual->PC++;
	}
	if(!procesoBloqueado){
		//finalizarPor(FIN_EJECUCION);
		log_inform("Finalizo ejecucion por fin de Quantum");
	}else {
		log_inform("Finalizo ejecucion por proceso bloqueado");
	}
}

int verificarTerminarEjecucion(){
	if(huboStackOver) {
		//finalizarPor(STACKOVERFLOW);
		huboStackOver = false;
		return -1;
	}
	else if(finPrograma){
		finPrograma = false;
		return -1;
	}
	else
		return 0;
}

char* pedirProximaInstruccionAMemoria(){

	t_intructions *index = pcbActual->indexCode;
	index += pcbActual->PC;
	t_intructions *instruccion = index;
	int comienzo = instruccion->start;
	int longitud = instruccion->offset;

	// Obtengo la dirección lógica de la instrucción a partir del índice de código:
	int page, offset, size;
	page = comienzo / tamanioPagina;
	offset = comienzo % tamanioPagina;
	size = longitud;

	log_inform("Solicitando Instrucción -> Pagina: %d - Offset: %d - Size: %d.", page, offset, size);

	unsigned char buff[BUFFER_CAPACITY];
	header_t header = protocol_header (OP_ME_SOLBYTPAG);
	header.usrpid = pcbActual->idProcess;
	header.msgsize = serial_pack (buff, "hhh", page, offset, size);
	packet_t packet = protocol_packet (header, buff);
	protocol_packet_send(packet, memoria_fd);

	//Recibo proxima instruccion a ejecutar
	packet_t packet2 = protocol_packet_receive(memoria_fd);

	if(packet2.header.opcode == OP_RESPONSE){
		strcpy(proximaInstruccion, (char*)packet2.payload);
		if(proximaInstruccion == NULL){
			printf("Fallo al pedido de proxima instruccion");
		} else {
		printf("Instruccion Obtenida: %s\n", proximaInstruccion);
		return proximaInstruccion;
		}
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
		log_inform("StackOverflow. Se finaliza el proceso");
		huboStackOver = true;
		return -1;
	}

	int pag = pcbActual->stackPointer / tamanioPagina;
	int offset = pcbActual->stackPointer % tamanioPagina;

	t_stack* lineaStack = list_get(pcbActual->stack, list_size(pcbActual->stack) - 1);
	if(lineaStack == NULL){
		lineaStack = t_stack_create();
		list_add(pcbActual->stack, lineaStack);
	}

	if(!esArgumento(identificador_variable)){ // Es una variable
		log_inform("ANSISOP_definirVariable %c", identificador_variable);
		t_var* nuevaVar = malloc(sizeof(t_var));
		nuevaVar->id = identificador_variable;
		nuevaVar->mempos.page = pag;
		nuevaVar->mempos.offset = offset;
		nuevaVar->mempos.size = sizeof(int);
		list_add(lineaStack->vars, nuevaVar);
	}
	else{ // Es un argumento.
		log_inform("ANSISOP_definirVariable (argumento) %c", identificador_variable);
		lineaStack->retPos = pcbActual->PC;
		t_var* nuevoArg = malloc(sizeof(t_var));
		nuevoArg->mempos.page = pag;
		nuevoArg->mempos.offset = offset;
		nuevoArg->mempos.size = sizeof(int);
		list_add(lineaStack->args, nuevoArg);
	}

	pcbActual->stackPointer += sizeof(int);
	int posAbsoluta = pcbActual->stackPointer - sizeof(int);
	log_inform("Posicion relativa de %c: %d %d %d", identificador_variable, pag, offset, sizeof(int));
	log_inform("Posicion absoluta de %c: %i", identificador_variable, posAbsoluta);
	return posAbsoluta;

}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){

	log_inform("ANSISOP_obtenerPosicion %c", identificador_variable);
	if(list_size(pcbActual->stack) == 0){
		log_report("No hay nada en el indice de stack");
		return EXIT_FAILURE;
	}

	t_puntero posicionAbsoluta;
	t_stack* contexto = list_get(pcbActual->stack, list_size(pcbActual->stack) - 1);

	if(!esArgumento(identificador_variable)){ // es una variable
		t_var* var_local;
		bool notFound = true;
		int i;
		for(i=0; i < list_size(contexto->vars); i++){
			var_local = list_get(contexto->vars, i);
			if(var_local->id == identificador_variable){
				notFound = false;
				break;
			}
		}
		if(notFound){
			log_report("No se encontro la variable %c en el stack", identificador_variable);
			return EXIT_FAILURE;
		}
		else{
			posicionAbsoluta = var_local->mempos.page * tamanioPagina + var_local->mempos.offset;
		}
	} // es un argumento
	else{
		if(identificador_variable -'0'> list_size(contexto->args)){
			return EXIT_FAILURE;
		}else{
			t_var* argumento = list_get(contexto->args, identificador_variable-'0');
			posicionAbsoluta = argumento->mempos.page * tamanioPagina + argumento->mempos.offset;
		}
	}
	log_inform("Posicion absoluta de %c: %d", identificador_variable, posicionAbsoluta);
	return posicionAbsoluta;

}

t_valor_variable dereferenciar(t_puntero direccion_variable){

	t_valor_variable valor;
	log_inform("ANSISOP_dereferenciar posicion: %d", direccion_variable);
	//calculo la posicion de la variable en el stack mediante el desplazamiento

	int page = (direccion_variable / tamanioPagina) + pcbActual->pagesCode;
	int offset = direccion_variable % tamanioPagina;
	int size = sizeof(int);

	unsigned char buff[BUFFER_CAPACITY];
	header_t header = protocol_header (OP_ME_SOLBYTPAG);
	header.usrpid = pcbActual->idProcess;
	header.msgsize = serial_pack(buff, "hhh", page, offset, size);
	packet_t packet = protocol_packet (header, buff);
	protocol_packet_send(packet, memoria_fd);

	//Recibo valor
	packet_t packet2 = protocol_packet_receive(memoria_fd);

	if(packet2.header.opcode == OP_RESPONSE){

		serial_unpack(packet2.payload , "h", &valor);
		if(buff == NULL){

			log_inform("Fallo al pedido de proxima instruccion");
		}

		log_inform("Valor obtenida: %d", valor);
		return valor;
	}

	return valor;
}

//Inserta una copia del valor en la variable ubicada en direccion_variable.
void asignar(t_puntero direccion_variable, t_valor_variable valor){

	log_inform("ANSISOP_asignar -> posicion var: %d - valor: %d", direccion_variable, valor);
	int page, offset, size;
	page = (direccion_variable / tamanioPagina) + pcbActual->pagesCode;
	offset = direccion_variable % tamanioPagina;
	size = sizeof(int);

	unsigned char buff[BUFFER_CAPACITY];
	header_t header = protocol_header (OP_ME_ALMBYTPAG);
	header.usrpid = pcbActual->idProcess;
	header.msgsize = serial_pack(buff, "hhh", page, offset, size);
	packet_t packet = protocol_packet (header, buff);
	protocol_packet_send(packet, memoria_fd);

	header.msgsize = serial_pack(buff, "h", valor);
	packet = protocol_packet (header, buff);
	protocol_packet_send(packet, memoria_fd);

	//Recibo respuesta de Memoria
	packet_t packetMemoria = protocol_packet_receive(memoria_fd);
    int res;
    serial_unpack(packetMemoria.payload, "h", &res);

	if(res == 0){
		log_inform("Fallo al asignar la variable");
	}else {
		log_inform("Variable asignada correctamente");
	}
}

void irAlLabel(t_nombre_etiqueta etiqueta){

	log_inform("ANSISOP_irALabel %s", etiqueta);
	t_puntero_instruccion numeroInstr = metadata_buscar_etiqueta(etiqueta, pcbActual->indexTag, pcbActual->tags);
	log_inform("Instruccion del irALAbel: %d", numeroInstr);
	if(numeroInstr == -1){
		log_report("No se encontro la etiqueta");
		return;
	}
	pcbActual->PC = numeroInstr;
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta){

	log_inform("ANSISOP_llamarSinRetorno %s", etiqueta);
	t_stack* nuevaLineaStack = t_stack_create();
	nuevaLineaStack->retPos = pcbActual->PC;
	list_add(pcbActual->stack, nuevaLineaStack);
	irAlLabel(etiqueta);
}

void finalizar(void){

	log_inform("ANSISOP_finalizar");
	//Obtengo contexto quitado de la lista y lo limpio.
	t_stack* contexto = list_remove(pcbActual->stack, list_size(pcbActual->stack) - 1);
	int i;
	if(contexto != NULL){
		pcbActual->stackPointer -= sizeof(int) * (list_size(contexto->args) + i<list_size(contexto->vars)); // Disminuyo stackPointer del pcb
		if(pcbActual->stackPointer >= 0){
			for(i=0; i<list_size(contexto->args); i++){ // Limpio lista de argumentos del contexto
				free(list_remove(contexto->args,i));
			}
			for(i=0; i<list_size(contexto->vars); i++){
				free(list_remove(contexto->vars, i));
			}
		}
		list_destroy(contexto->args);
		list_destroy(contexto->vars);
	}
	if(list_size(pcbActual->stack) == 0){
		finPrograma = true;
		log_inform("Finalizó la ejecucion del programa.");
		//finalizarPor(FIN_PROCESO);
	}else{
		pcbActual->PC = contexto->retPos;
	}
	free(contexto);
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

