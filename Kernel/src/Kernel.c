#include "Kernel.h"
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <parser/parser.h>
#include <parser/metadata_program.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "serial.h"
#include "structures.h"
#include "thread.h"
#include "log.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>
#include <semaphore.h>

void init_server(t_kernel* kernel, socket_t mem_fd, socket_t fs_fd) {
	t_list* pcb_list = list_create();

	fdset_t read_fds, all_fds = socket_set_create();
	socket_set_add(mem_fd, &all_fds);
//	socket_set_add(fs_fd, &all_fds);

	socket_t sv_sock = socket_init(NULL, kernel->puerto_prog);
	socket_set_add(sv_sock, &all_fds);

	while(true) {
		read_fds = all_fds;
		fdcheck(select(read_fds.max + 1, &read_fds.set, NULL, NULL, NULL));

		for(socket_t i = 0; i <= all_fds.max; i++) {
			if(!FD_ISSET(i, &read_fds.set)) continue;
			if(i == sv_sock) {
				socket_t cli_sock = socket_accept(sv_sock);
				process_t cli_process = protocol_handshake_receive(cli_sock);
				if(cli_process == CONSOLE) {
					socket_set_add(cli_sock, &all_fds);
					log_inform("Received handshake from %s\n", get_process_name(cli_process));
				}
				else if(cli_process == CPU){
					socket_set_add(cli_sock, &all_fds);
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
				packet_t program = protocol_packet_receive(i);
				if(program.header.opcode == OP_UNDEFINED) {
					socket_close(i);
					FD_CLR(i, &all_fds.set);
					continue;
				}else{
					if(program.header.opcode == OP_NEW_PROGRAM && pcb_list->elements_count < kernel->grado_multiprog) {
						for(socket_t j = 0; j <= all_fds.max; j++) {
							if(!FD_ISSET(j, &all_fds.set) || j == sv_sock || j == i) continue;
								gestion_datos_pcb((char*)program.payload, j, mem_fd, pcb_list);
						}
					}else{
						if(pcb_list->elements_count >= kernel->grado_multiprog){
							packet_t packet = protocol_packet(protocol_header(OP_KE_REFUSEPROGRAM));
							protocol_packet_send(packet, i);
							printf("Ya hay un programa en ejecución\n");
						}
					}
				}
			}
		}
	}
}

void terminal() {
	title("Consola");
	// TODO
}

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(KERNEL);
	title("KERNEL");

	t_kernel* kernel = malloc(sizeof(t_kernel));
	leerConfiguracionKernel(kernel, argv[1]);
	
	title("Conexión");
	printf("Estableciendo conexión con la Memoria...");
	int memoria_fd = socket_connect(kernel->ip_memoria, kernel->puerto_memoria);
	protocol_handshake_send(memoria_fd);
	printf("\33[2K\rConectado a la Memoria en %s:%s\n", kernel->ip_memoria, kernel->puerto_memoria);

//	Lo comento para que no joda. Total por ahora no lo necesitamos.
//	printf("Estableciendo conexión con el File System...");
	int fs_fd = 0; // socket_connect(kernel->ip_fs, kernel->puerto_fs);
//	protocol_send_handshake(fs_fd);
//	printf("\33[2K\rConectado al File System en %s:%s\n", kernel->ip_fs, kernel->puerto_fs);

	thread_create(terminal);

	init_server(kernel, memoria_fd, fs_fd);

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
		printf("STACK_SIZE: %i\n", kernel->stack_size);
}

t_pcb* crear_pcb_proceso(t_metadata_program* program) {
	t_pcb *element = alloc(sizeof(t_pcb));

	element->idProcess = 0;
	element->PC = program->instruccion_inicio;
	element->status = NEW;
	element->pagesCode = 0;
	element->instructions = program->instrucciones_size;
	element->indexCode = program->instrucciones_serializado;

	const int NUM_TAGS = program->cantidad_de_etiquetas + program->cantidad_de_funciones;
	element->indexTag = alloc(NUM_TAGS * sizeof(t_programTag));

	char tag[LINE_SIZE];
	char *p = tag, *q = program->etiquetas;
	int numtag = 0;

	while(numtag < NUM_TAGS) {
		*p++ = *q++;
		if(*(p-1) != '\0') continue;
		element->indexTag[numtag].name = string_duplicate(tag);
		element->indexTag[numtag].PC = metadata_buscar_etiqueta(tag, program->etiquetas, program->etiquetas_size);
		p = tag;
		q += sizeof(t_puntero_instruccion);
		numtag++;
	}

	element->indexStack = list_create();
	element->stackPointer = 0;
	element->exitCode = 0;

	return element;
}

void gestion_datos_pcb(char* buffer, socket_t cli_socket, socket_t server_socket, t_list* lista) {

	t_metadata_program* dataProgram = metadata_desde_literal(buffer);
	t_pcb* pcb = crear_pcb_proceso(dataProgram);
	list_add(lista, pcb);
	unsigned char buff[BUFFER_CAPACITY];

	packet_t packet = protocol_packet(protocol_header(OP_ME_SOLBYTPAG));
	protocol_packet_send(packet, server_socket);

	//Hasta que memoria responda el OK

	/*packet = protocol_packet_receive(server_socket);
	 *
	if (packet.header.usrpid == ACCEPT){*/

		header_t header_pcb = protocol_header (OP_KE_SENDINGDATA);
		header_pcb.msgsize = serial_pack (buff, "hh", pcb->idProcess, pcb->pagesCode);
		packet_t packet_pcb = protocol_packet (header_pcb, buff);

		/*protocol_packet_send(packet_pcb, server_socket);*/

		protocol_packet_send(packet_pcb, cli_socket);

	/*}*/

	metadata_destruir(dataProgram);

}
