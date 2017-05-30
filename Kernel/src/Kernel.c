#include "Kernel.h"


void init_server(socket_t mem_fd, socket_t fs_fd) {

    fdset_t read_fds, all_fds = socket_set_create();
    socket_set_add(mem_fd, &all_fds);
//  socket_set_add(fs_fd, &all_fds);
    socket_t sv_sock = socket_init(NULL, kernel->puerto_prog);
    socket_set_add(sv_sock, &all_fds);

    while(true) {
        read_fds = all_fds;
        fdcheck(select(read_fds.max + 1, &read_fds.set, NULL, NULL, NULL));

        for(socket_t i = 0; i <= all_fds.max; i++) {
            if(!FD_ISSET(i, &read_fds.set)) continue;
            if(i == sv_sock) {
                socket_t cli_sock = socket_accept(sv_sock);
                header_t header = protocol_handshake_receive(cli_sock);
                process_t cli_process = header.syspid;
                if(cli_process == CONSOLE) {
                    socket_set_add(cli_sock, &all_fds);
                    t_client* cliente = alloc(sizeof(t_client));
                    cliente->clientID = cli_sock;
                    cliente->process= CONSOLE;
                    list_add (consolas_conectadas, cliente);
                    log_inform("Received handshake from %s\n", get_process_name(cli_process));
                }
                else if(cli_process == CPU){
                    socket_set_add(cli_sock, &all_fds);
                    t_client* cliente = alloc(sizeof(t_client));
                    cliente->clientID = cli_sock;
                    cliente->status = READY;
                    cliente->process= CPU;
                    list_add (cpu_conectadas, cliente);
                    log_inform("Received handshake from %s\n", get_process_name(cli_process));

                    log_inform("Envio stack size (%i) a CPU", kernel->stack_size);
                    unsigned char buff[BUFFER_CAPACITY];
                    header_t header = protocol_header(OP_KE_SEND_STACK_SIZE);
                    header.msgsize = serial_pack(buff, "h", kernel->stack_size);
                    packet_t packet = protocol_packet(header, buff);
                    protocol_packet_send(packet, cli_sock);
                }
                else {
                    socket_close(cli_sock);
                }
            } else {
            	t_client* process = buscar_proceso (i);
            		if (process->process == CONSOLE){
            			packet_t program = protocol_packet_receive(process->clientID);
                		if(program.header.opcode == OP_UNDEFINED) {
                			socket_close(i);
                			FD_CLR(i, &all_fds.set);
                			continue;
                		}else{
                			if(program.header.opcode == OP_NEW_PROGRAM) {
                				pthread_mutex_lock(&mutex_planificacion);
                				gestion_datos_newPcb(program, mem_fd, i);
                				pthread_mutex_unlock(&mutex_planificacion);
                			}
                		}
            		}

                	if (process->process == CPU){
                		pthread_mutex_lock(&mutex_planificacion);
            			packet_t cpu_call = protocol_packet_receive(process->clientID);
            			pthread_mutex_unlock(&mutex_planificacion);
                	}
            }
        }
        pthread_mutex_lock(&mutex_planificacion);
        planificacion();
        pthread_mutex_unlock(&mutex_planificacion);
    }
}

void terminal() {
    title("Consola");

    while(true) {
            char *argument = input(command);
            if(streq(command, "processlist")) {
                for (int i = 0; i < pcb_exec->elements_count; i++){
                    t_pcb* aux = list_get (pcb_exec, i);
                    printf("PID: %d\n", aux->idProcess);
                }
                for (int i = 0; i < pcb_exit->elements_count; i++){
                    t_pcb* aux = list_get (pcb_exit, i);
                    printf("PID: %d\n", aux->idProcess);
                }
                for (int i = 0; i < pcb_new->elements_count; i++){
                    t_pcb* aux = list_get (pcb_new, i);
                    printf("PID: %d\n", aux->idProcess);
                }
                for (int i = 0; i < pcb_block->elements_count; i++){
                    t_pcb* aux = list_get (pcb_block, i);
                    printf("PID: %d\n", aux->idProcess);
                }
                for (int i = 0; i < pcb_ready->elements_count; i++){
                    t_pcb* aux = list_get (pcb_ready, i);
                    printf("PID: %d\n", aux->idProcess);
                }
            } else if(streq(command, "status")) {
                int pid = atoi(argument);
                                for (int i = 0; i < pcb_exec->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_exec, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "EXECUTING");
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_exit->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_exit, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "EXIT");
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_new->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_new, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "NEW");
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_block->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_block, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "BLOCK");
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_ready->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_ready, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "READY");
                                        break;
                                    }
                                }
            } else if(streq(command, "kill")) {
                int pid = atoi(argument);
                bool stop = 0;
                    while (!stop){
                                for (int i = 0; i < pcb_new->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_new, i);
                                    if (aux->idProcess == pid){
                                        aux = list_remove (pcb_new, i);
                                        aux->exitCode = -2;
                                        aux->status = EXIT;
                                        list_add (pcb_exit, aux);
                                        stop = 1;
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_block->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_block, i);
                                    if (aux->idProcess == pid){
                                        aux = list_remove (pcb_block, i);
                                        aux->exitCode = -2;
                                        aux->status = EXIT;
                                        list_add (pcb_exit, aux);
                                        stop = 1;
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_ready->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_ready, i);
                                    if (aux->idProcess == pid){
                                        aux = list_remove (pcb_ready, i);
                                        aux->exitCode = -2;
                                        aux->status = EXIT;
                                        list_add (pcb_exit, aux);
                                        stop = 1;
                                        break;
                                    }
                                }
                    }
            } else if(streq(command, "stop")) {
            	pthread_mutex_lock(&mutex_planificacion);
            } else if(streq(command, "restart")) {
            	pthread_mutex_unlock(&mutex_planificacion);
            }else {
                puts("Comando no reconocido. Escriba 'help' para ayuda.");
            }
        }
}

int main(int argc, char **argv) {

    guard(argc == 2, "Falta indicar ruta de archivo de configuración");
    set_current_process(KERNEL);
    title("KERNEL");

    kernel = malloc(sizeof(t_kernel));
    leerConfiguracionKernel(kernel, argv[1]);

	pthread_mutex_init(&mutex_planificacion, NULL);
	pcb_ready = list_create();
    pcb_new = list_create();
    pcb_exec = list_create();
    pcb_block = list_create();
    pcb_exit = list_create();
    consolas_conectadas = list_create();
    cpu_conectadas = list_create ();
    cpu_executing = list_create();

    title("Conexión");
    printf("Estableciendo conexión con la Memoria...");
    socket_t memoria_fd = socket_connect(kernel->ip_memoria, kernel->puerto_memoria);
    protocol_handshake_send(memoria_fd);
    printf("\33[2K\rConectado a la Memoria en %s:%s\n", kernel->ip_memoria, kernel->puerto_memoria);

    packet_t packet = protocol_packet_receive(memoria_fd);
    serial_unpack(packet.payload, "h", &kernel->page_size);

//    Lo comento para que no joda. Total por ahora no lo necesitamos.
//    printf("Estableciendo conexión con el File System...");
    int fs_fd = 0; // socket_connect(kernel->ip_fs, kernel->puerto_fs);
//    protocol_send_handshake(fs_fd);
//    printf("\33[2K\rConectado al File System en %s:%s\n", kernel->ip_fs, kernel->puerto_fs);

    thread_create(terminal);
    init_server(memoria_fd, fs_fd);

    free(kernel);
    return 0;
}


void leerConfiguracionKernel(t_kernel* kernel, char* path){

        t_config* config = config_create(path);

        kernel->puerto_prog = config_get_string_value(config, "PUERTO_PROG");
        kernel->puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
        kernel->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
        kernel->puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
        kernel->ip_fs = config_get_string_value(config, "IP_FS");
        kernel->puerto_fs = config_get_string_value(config, "PUERTO_FS");
        kernel->quantum = config_get_int_value(config, "QUANTUM");
        kernel->quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
        kernel->algoritmo = config_get_string_value(config, "ALGORITMO");
        kernel->grado_multiprog = config_get_int_value(config, "GRADO_MULTIPROG");
        char** semaforos = config_get_array_value(config, "SEM_IDS");
        for(int i=0; i<3; i++){
            strcpy (kernel->sem_ids[i].__size, semaforos[i]);
        }
        char** semaforos_init = config_get_array_value(config, "SEM_INIT");
        for(int i=0; i<3; i++){
            kernel->sem_init[i] = atoi (semaforos_init[i]);
        }
        kernel->shared_vars = config_get_array_value (config, "SHARED_VARS");
        int counter = 0; while (kernel->shared_vars[counter]) counter++;
        kernel->shared_values = realloc (kernel->shared_values, counter * sizeof(int));
        for(int i=0; i<counter; i++){
            kernel->shared_values[i] = 0;
        }
        kernel->stack_size = config_get_int_value(config, "STACK_SIZE");

        title("Configuración");
        printf("PUERTO_PROG: %s\n", kernel->puerto_prog);
        printf("PUERTO_CPU: %s\n", kernel->puerto_cpu);
        printf("IP_MEMORIA: %s\n", kernel->ip_memoria);
        printf("PUERTO_MEMORIA: %s\n", kernel->puerto_memoria);
        printf("IP_FS: %s\n", kernel->ip_fs);
        printf("PUERTO_FS: %s\n", kernel->puerto_fs);
        printf("QUANTUM: %i\n", kernel->quantum);
        printf("QUANTUM_SLEEP: %i\n", kernel->quantum_sleep);
        printf("ALGORITMO: %s\n", kernel->algoritmo);
        printf("GRADO_MULTIPROG: %i\n", kernel->grado_multiprog);
        printf("SEM_IDS:[%s, %s ,%s]\n", kernel->sem_ids[0].__size, kernel->sem_ids[1].__size, kernel->sem_ids[2].__size);
        printf("SEM_INIT:[%d, %d ,%d]\n", kernel->sem_init[0], kernel->sem_init[1], kernel->sem_init[2]);
        printf("SHARED_VARS:[!%s, !%s ,!%s]\n", kernel->shared_vars[0], kernel->shared_vars[1], kernel->shared_vars[2]);
        printf("STACK_SIZE: %i\n", kernel->stack_size);
}


t_pcb* crear_pcb_proceso(t_metadata_program* program) {
    t_pcb *element = alloc(sizeof(t_pcb));
    generatorPid += 1;

    element->idProcess = generatorPid;
    element->PC = program->instruccion_inicio;
    element->status = NEW;
    element->pagesCode = 0;
    element->quantum = kernel->quantum;
    element->instructions = program->instrucciones_size;
    element->indexCode = program->instrucciones_serializado;

    element->tags = program->cantidad_de_etiquetas + program->cantidad_de_funciones;
    element->indexTag = alloc(element->tags * sizeof(t_programTag));

    char tag[LINE_SIZE];
    char *p = tag, *q = program->etiquetas;
    int numtag = 0;

    while(numtag < element->tags) {
        *p++ = *q++;
        if(*(p-1) != '\0') continue;
        element->indexTag[numtag].name = string_duplicate(tag);
        element->indexTag[numtag].PC = metadata_buscar_etiqueta(tag, program->etiquetas, program->etiquetas_size);
        p = tag;
        q += sizeof(t_puntero_instruccion);
        numtag++;
    }

    element->stackPointer = 0;
    element->stack = list_create();

    element->exitCode = 0;

    return element;
}

void gestion_datos_newPcb(packet_t program, socket_t server_socket, socket_t console_socket) {
    unsigned char buffer[BUFFER_CAPACITY];

    t_metadata_program* dataProgram = metadata_desde_literal((char*)program.payload);
    t_pcb* pcb = crear_pcb_proceso(dataProgram);


    header_t header_pid = protocol_header(OP_KE_SEND_PID);
    header_pid.usrpid = pcb->idProcess;
    packet_t packet = protocol_packet(header_pid);
    protocol_packet_send(packet, console_socket);
    list_add (pcb_new, pcb);

    if ((pcb_ready->elements_count+pcb_exec->elements_count+pcb_block->elements_count) < kernel->grado_multiprog){
    	//Obtengo paginas requeridas para el proceso y se lo solicito a memoria
        int pages = (program.header.msgsize/kernel->page_size) + (program.header.msgsize % kernel->page_size != 0) + kernel->stack_size;

        header_t headerMemoria = protocol_header(OP_ME_INIPRO, serial_pack(buffer, "H", pages));
        headerMemoria.usrpid = pcb->idProcess;
        packet_t packetMemoria = protocol_packet(headerMemoria, buffer);
        protocol_packet_send(packetMemoria, server_socket);

        //Espero que responda memoria, si está OK (=0), continuo
        packetMemoria = protocol_packet_receive(server_socket);
        int res;
        serial_unpack(packetMemoria.payload, "h", &res);
        if (res) {
            pcb->pagesCode = pages - kernel->stack_size;
            pcb->stackPointer = pages - kernel->stack_size;
            t_pcb* pcb = list_remove (pcb_new, 0);
			list_add(pcb_ready, pcb);

           //protocol_packet_send(program, server_socket);

            header_t header_stack = protocol_header (OP_KE_SEND_STACK);
            header_stack.msgsize = serial_pack_stack(pcb->stack, buffer);
            packet_t packet_stack = protocol_packet (header_stack, buffer);

            //protocol_packet_send(packet_stack, server_socket);
        }
        else {
            pcb->exitCode = -1;
            list_add(pcb_exit, pcb);
        }
    }
}


void planificacion (){
    while (!list_is_empty(pcb_ready) && !list_is_empty(cpu_conectadas)){
        unsigned char buff[BUFFER_CAPACITY];
        t_pcb* pcbToExec = list_remove(pcb_ready, 0);

        for(int i= 0; i < pcbToExec->instructions; i++){
        	printf("%d\n%d\n", (pcbToExec->indexCode+i)->start, (pcbToExec->indexCode+i)->offset);
        }

        header_t header_pcb = protocol_header (OP_KE_SEND_PCB);
        header_pcb.msgsize = serial_pack_pcb(pcbToExec, buff);
        packet_t packet_pcb = protocol_packet (header_pcb, buff);

        t_client* cpu = list_remove(cpu_conectadas, 0);
        protocol_packet_send(packet_pcb, cpu->clientID);
        cpu->status = EXEC;

        list_add(pcb_exec, pcbToExec);
        list_add (cpu_executing, cpu);
    }
}

t_client* buscar_proceso (socket_t client){
	t_client* aux;

	for (int i = 0; i < cpu_conectadas->elements_count; i++){
		aux = list_get (cpu_conectadas, i);
		if (aux->clientID == client) break;
	}

	for (int i = 0; i < cpu_executing->elements_count; i++){
	 	aux = list_get (cpu_executing, i);
	 	if (aux->clientID == client) break;
	}

	for (int i = 0; i < consolas_conectadas->elements_count; i++){
		aux = list_get (consolas_conectadas, i);
		if (aux->clientID == client) break;
	}

	return aux;
}
