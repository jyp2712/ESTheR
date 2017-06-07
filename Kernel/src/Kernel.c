#include "Kernel.h"
#include "configuration.h"

int generatorPid = 0;
t_kernel* config;

void init_server(socket_t mem_fd, socket_t fs_fd) {

    fdset_t read_fds, all_fds = socket_set_create();
    socket_set_add(mem_fd, &all_fds);
//  socket_set_add(fs_fd, &all_fds);
    socket_t sv_sock = socket_init(NULL, config->puerto_prog);
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
                    cliente->pids = list_create();
                    list_add (consolas_conectadas, cliente);
                    log_inform("Received handshake from %s", get_process_name(cli_process));
                }
                else if(cli_process == CPU){
                    socket_set_add(cli_sock, &all_fds);
                    t_client* cliente = alloc(sizeof(t_client));
                    cliente->clientID = cli_sock;
                    cliente->process= CPU;
                    list_add (cpu_conectadas, cliente);
                    log_inform("Received handshake from %s", get_process_name(cli_process));

                    log_inform("Envio stack size (%i) y tamaño de pagina (%i) a CPU", config->stack_size, config->page_size);
                    unsigned char buff[BUFFER_CAPACITY];
                    header_t header = protocol_header(OP_KE_SEND_STACK_SIZE);
                    header.msgsize = serial_pack(buff, "hh", config->stack_size, config->page_size);
                    packet_t packet = protocol_packet(header, buff);
                    protocol_packet_send(packet, cli_sock);
                }
                else {
                    socket_close(cli_sock);
                }
            }
            else {
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
                				gestion_datos_newPcb(program, process);
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
        planificacion(mem_fd);
        pthread_mutex_unlock(&mutex_planificacion);
    }
}

int main(int argc, char **argv) {
    guard(argc == 2, "Falta indicar ruta de archivo de configuración");
    config = get_config(argv[1]);
    //config = get_config("metadata");

    set_current_process(KERNEL);
    title("KERNEL");

	pthread_mutex_init(&mutex_planificacion, NULL);
	pcb_ready = list_create();
    pcb_new = list_create();
    pcb_exec = list_create();
    pcb_block = list_create();
    pcb_exit = list_create();
    consolas_conectadas = list_create();
    cpu_conectadas = list_create ();
    cpu_executing = list_create();
    deadpid = list_create();
    codes_ms = list_create();

    title("Conexión");
    printf("Estableciendo conexión con la Memoria...");
    socket_t memoria_fd = socket_connect(config->ip_memoria, config->puerto_memoria);
    protocol_handshake_send(memoria_fd);
    printf("\33[2K\rConectado a la Memoria en %s:%s\n", config->ip_memoria, config->puerto_memoria);

    packet_t packet = protocol_packet_receive(memoria_fd);
    serial_unpack(packet.payload, "h", &config->page_size);

//    Lo comento para que no joda. Total por ahora no lo necesitamos.
//    printf("Estableciendo conexión con el File System...");
    int fs_fd = 0; // socket_connect(kernel->ip_fs, kernel->puerto_fs);
//    protocol_send_handshake(fs_fd);
//    printf("\33[2K\rConectado al File System en %s:%s\n", kernel->ip_fs, kernel->puerto_fs);

    thread_create(terminal);
    init_server(memoria_fd, fs_fd);

    free(config);
    return 0;
}

t_pcb* crear_pcb_proceso(t_metadata_program* program) {
    t_pcb *element = alloc(sizeof(t_pcb));
    generatorPid += 1;

    element->idProcess = generatorPid;
    element->PC = program->instruccion_inicio;
    element->status = NEW;
    element->pagesCode = 0;
    element->quantum = config->quantum;
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

void gestion_datos_newPcb(packet_t program, t_client* console) {

    t_metadata_program* dataProgram = metadata_desde_literal((char*)program.payload);
    t_pcb* pcb = crear_pcb_proceso(dataProgram);

    t_code_ms* code = alloc(sizeof(t_code_ms));
    code->codigo = alloc(program.header.msgsize);
    memcpy(code->codigo, program.payload, program.header.msgsize);
    code->size = program.header.msgsize;
    list_add(codes_ms, code);

    header_t header_pid = protocol_header(OP_KE_SEND_PID);
    header_pid.usrpid = pcb->idProcess;
    packet_t packet = protocol_packet(header_pid);
    protocol_packet_send(packet, console->clientID);
    list_add(console->pids, (int*)pcb->idProcess);

    list_add (pcb_new, pcb);
}

void planificacion (socket_t server_socket){
    unsigned char buffer[BUFFER_CAPACITY];

	if ((list_size(pcb_ready) + list_size(pcb_exec) + list_size(pcb_block)) < config->grado_multiprog && !list_is_empty(pcb_new)){
		t_code_ms* code = list_remove(codes_ms, 0);
		//Obtengo paginas requeridas para el proceso y se lo solicito a memoria
        int pages = (code->size/config->page_size) + (code->size % config->page_size != 0) + config->stack_size;

        t_pcb* pcb = list_remove (pcb_new, 0);

        header_t headerMemoria = protocol_header(OP_ME_INIPRO, serial_pack(buffer, "h", pages));
        headerMemoria.usrpid = pcb->idProcess;
        packet_t packetMemoria = protocol_packet(headerMemoria, buffer);
        protocol_packet_send(packetMemoria, server_socket);

        log_inform("Usrpid: %d and pages requested: %d.", headerMemoria.usrpid, pages);

        //Espero que responda memoria, si está OK (=0), continuo
        packetMemoria = protocol_packet_receive(server_socket);
        int res;
        serial_unpack(packetMemoria.payload, "h", &res);
        if (res) {
            pcb->pagesCode = pages - config->stack_size;
    		list_add(pcb_ready, pcb);

    		int tam = 0;
			int size;
			log_inform("Pages code: %i", pcb->pagesCode);
    		for (int pag = 0; pag < pcb->pagesCode; pag++){
    			if (pag < pcb->pagesCode - 1){
    				size = config->page_size;
    			}
    			else{
    				size = (code->size % config->page_size);
    				log_inform("Size del codigo restante: %i", size);
    			}

    			header_t header_code = protocol_header(OP_ME_ALMBYTPAG);
    			header_code.usrpid = pcb->idProcess;
    			header_code.msgsize = serial_pack(buffer, "hhh", pag, 0, size);
    			packet_t packet_code = protocol_packet(header_code, buffer);
    			protocol_packet_send(packet_code, server_socket);

    			memcpy(buffer, code->codigo+tam, size);

    			log_inform("%s", buffer);

    			header_code.msgsize = size;
    			packet_code = protocol_packet(header_code, buffer);
    			protocol_packet_send(packet_code, server_socket);
    			tam += size;

				packet_t packetMemoria = protocol_packet_receive(server_socket);

				int res;
				serial_unpack(packetMemoria.payload, "h", &res);
				if(res == 0){
					log_inform("Fallo al guardar el codigo en la pagina %i", pag+1);
				}else {
					log_inform("Codigo guardado en la pagina %i", pag+1);
				}
    		}

    		header_t header_stack = protocol_header (OP_ME_ALMBYTPAG);
    		header_stack.usrpid = pcb->idProcess;
    		header_stack.msgsize = serial_pack(buffer, "hhh", pcb->stackPointer, 0, list_size(pcb->stack));
    		packet_t packet_stack = protocol_packet (header_stack, buffer);
    		protocol_packet_send(packet_stack, server_socket);

    		header_stack.msgsize = serial_pack_stack(pcb->stack, buffer);
    		packet_stack = protocol_packet (header_stack, buffer);
    		protocol_packet_send(packet_stack, server_socket);

    		packetMemoria = protocol_packet_receive(server_socket);
        }else {
            pcb->exitCode = -1;
            list_add(pcb_exit, pcb);
        }
    }

    while (!list_is_empty(pcb_ready) && !list_is_empty(cpu_conectadas)){
        unsigned char buff[BUFFER_CAPACITY];
        t_pcb* pcbToExec = list_remove(pcb_ready, 0);

        header_t header_pcb = protocol_header (OP_KE_SEND_PCB);
        header_pcb.msgsize = serial_pack_pcb(pcbToExec, buff);
        packet_t packet_pcb = protocol_packet (header_pcb, buff);

        t_client* cpu = list_remove(cpu_conectadas, 0);
        protocol_packet_send(packet_pcb, cpu->clientID);

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

void gestion_syscall(packet_t cpu_syscall, t_client* cpu, socket_t mem_socket){
    unsigned char buffer[BUFFER_CAPACITY];

	switch (cpu_syscall.header.opcode){
		case OP_CPU_PROGRAM_END:{
									t_pcb* pcb = alloc(sizeof(t_pcb));
									serial_unpack_pcb(pcb, cpu_syscall.payload);
									bool getPcb (t_pcb *pcbExec){
									return (pcb->idProcess == pcbExec->idProcess);
									}
									t_pcb* aux = list_remove_by_condition(pcb_exec, (void*)getPcb);
									free(aux);
									list_add(pcb_exit, pcb);

									bool getClient (t_client *console){
										bool ret = false;
										int* pidConsole;
										for(int i = 0; i < list_size(console->pids); i++){
											pidConsole = list_get(console->pids, 0);
											if ((int*)pcb->idProcess == pidConsole){
												ret = true;
												break;
											}
										}
										free(pidConsole);
										return ret;
									}
									t_client* console = list_find(consolas_conectadas, (void*)getClient);

									header_t header_program_end = protocol_header(OP_KE_PROGRAM_END, serial_pack(buffer, "h", pcb->exitCode));
								    header_program_end.usrpid = pcb->idProcess;
									packet_t packet_program_end = protocol_packet(header_program_end, buffer);
								    protocol_packet_send(packet_program_end, console->clientID);
								    protocol_packet_send(packet_program_end, mem_socket);

									//restoreCPU(cpu);
									break;
		}
		case OP_CPU_SEMAPHORE:{		t_pcb* pcb = alloc(sizeof(t_pcb));
									serial_unpack_pcb(pcb, cpu_syscall.payload);
									bool getPcb (t_pcb *pcbExec){
									return (pcb->idProcess == pcbExec->idProcess);
									}
									t_pcb* aux = list_remove_by_condition(pcb_exec, (void*)getPcb);
									free(aux);
									packet_t packet_sem = protocol_packet_receive(cpu->clientID);
									char sem[20];
									serial_unpack(packet_sem.payload, "20s", &sem);
									int i=0;int semValue;
									while(config->sem_ids[i]){
										if (strcmp(config->sem_ids[i], sem) == 0) break;
									}
									if (true/*packet_sem.header.opcode == wait*/){
										sem_wait(&config->sem_ansisop[i]);
										sem_getvalue(&config->sem_ansisop[i], &semValue);
										if (semValue>0){
											list_add(pcb_ready, pcb);
										}else{
											list_add(pcb_block, pcb);
											list_add(config->solicitudes_sem[i], pcb);
										}
									}else{
										sem_post(&config->sem_ansisop[i]);
										sem_getvalue(&config->sem_ansisop[i], &semValue);
										list_add(pcb_block, pcb);
										list_add(config->solicitudes_sem[i], pcb);
										t_pcb* pcbReady = list_remove(config->solicitudes_sem[i], 0);
										list_add(pcb_ready, pcb);
										list_add(config->solicitudes_sem[i], pcb);
										bool getPcb (t_pcb *pcbExec){
											return (pcbReady->idProcess == pcbExec->idProcess);
										}
										t_pcb* aux = list_remove_by_condition(pcb_block, (void*)getPcb);
										free(aux);
									}

									//restore(cpu);
									break;
		}
		case OP_CPU_SHARED_VAR:{	t_pcb* pcb = alloc(sizeof(t_pcb));
									serial_unpack_pcb(pcb, cpu_syscall.payload);
									bool getPcb (t_pcb *pcbExec){
										return (pcb->idProcess == pcbExec->idProcess);
									}
									t_pcb* aux = list_remove_by_condition(pcb_exec, (void*)getPcb);
									free(aux);
									packet_t packet_shared_var = protocol_packet_receive(cpu->clientID);
									char shared_var[20];
									serial_unpack(packet_shared_var.payload, "20s", &shared_var);
									int i=0;
									while(config->shared_vars[i]){
										if (strcmp(config->sem_ids[i], shared_var) == 0) break;
									}
									if (true/*packet_shared_var.header.opcode == get shared value*/){
										header_t header_shared_var = protocol_header(OP_KE_SEND_SHAREDVALUE, serial_pack(buffer, "h", config->shared_values[i]));
										packet_t packet_shared_var = protocol_packet(header_shared_var, buffer);
										protocol_packet_send(packet_shared_var, cpu->clientID);
									}else{
										int value;
										serial_unpack(packet_shared_var.payload+20, "h", &value);
										config->shared_values[i] = value;
									}

									//restore(cpu);
									break;
		}
	}
}
