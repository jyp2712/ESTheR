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


#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>
#include <semaphore.h>

void init_server(const char *port, socket_t mem_fd, socket_t fs_fd) {
	char buffer[BUFFER_CAPACITY];

	fdset_t read_fds, all_fds = socket_set_create();
	socket_set_add(mem_fd, &all_fds);
//	socket_set_add(fs_fd, &all_fds);

	socket_t sv_sock = socket_init(NULL, port);
	socket_set_add(sv_sock, &all_fds);

	while(true) {
		read_fds = all_fds;
		fdcheck(select(read_fds.max + 1, &read_fds.set, NULL, NULL, NULL));

		for(socket_t i = 0; i <= all_fds.max; i++) {
			if(!FD_ISSET(i, &read_fds.set)) continue;
			if(i == sv_sock) {
				socket_t cli_sock = socket_accept(sv_sock);
				process_t cli_process = protocol_handshake_receive(cli_sock);
				if(cli_process == CONSOLE || cli_process == CPU) {
					socket_set_add(cli_sock, &all_fds);
					printf("Recibido handshake de%s\n", cli_process == CPU ? "l CPU" : " la Consola");
				} else {
					socket_close(cli_sock);
				}
			} else {
				if(socket_receive_string(buffer, i) == 0 ) {
					socket_close(i);
					FD_CLR(i, &all_fds.set);
					continue;
				}

				printf("Recibido mensaje: \"%s\"\n", buffer);

				for(socket_t j = 0; j <= all_fds.max; j++) {
					if(!FD_ISSET(j, &all_fds.set) || j == sv_sock || j == i) continue;
					socket_send_string(buffer, j);
				}
			}
		}
	}
}

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(KERNEL);

	t_kernel* kernel = malloc(sizeof(t_kernel));
	leerConfiguracionKernel(kernel, argv[1]);
	
	puts("Conectándose a la Memoria...");
	int memoria_fd = socket_connect(kernel->ip_memoria, kernel->puerto_memoria);
	protocol_handshake_send(memoria_fd);
	puts("Conectado a la Memoria.");

//	Lo comento para que no joda. Total por ahora no lo necesitamos.
//	puts("Conectándose al File System...");
	int fs_fd = 0; // socket_connect(kernel->ip_fs, kernel->puerto_fs);
//	protocol_send_handshake(fs_fd);
//	puts("Conectado al File System.");

	gestion_datos_pcb();

	init_server(kernel->puerto_prog, memoria_fd, fs_fd);

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

		printf("---------------Mi configuración---------------\n");
		printf("PUERTO_PROG: %s\n", kernel->puerto_prog);
		printf("PUERTO_CPU: %s\n", kernel->puerto_cpu);
		printf("IP_MEMORIA: %s\n", kernel->ip_memoria);
		printf("PUERTO_MEMORIA: %s\n", kernel->puerto_memoria);
		printf("IP_FS: %s\n", kernel->ip_fs);
		printf("PUERTO_FS: %s\n", kernel->puerto_fs);
		printf("QUANTUM: %i\n", kernel->quantum);
		printf("QUANTUM_SLEEP: %i\n", kernel->quantum_sleep);
		printf("ALGORITMO: %s\n", kernel->algoritmo);
		printf("SEM_IDS:[%s, %s ,%s]\n", kernel->sem_ids[0].__size, kernel->sem_ids[1].__size, kernel->sem_ids[2].__size);
		printf("SEM_INIT:[%d, %d ,%d]\n", kernel->sem_init[0], kernel->sem_init[1], kernel->sem_init[2]);
		printf("STACK_SIZE: %i\n", kernel->stack_size);
		printf("----------------------------------------------\n");
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

void gestion_datos_pcb() {
	char buffer[PROGRAM_SIZE];
	readfile("completo.ansisop", buffer);

	t_metadata_program* dataProgram = metadata_desde_literal(buffer);
	t_pcb* pcb = crear_pcb_proceso(dataProgram);

	printf("PID: %d\n", pcb->idProcess);
	printf("PC: %d\n", pcb->PC);
	printf("Status: %d\n", pcb->status);
	printf("Paginas de codigo: %d\n", pcb->pagesCode);
	printf("Cantidad de instrucciones: %d\n", pcb->instructions);
	for (int i =0; i < pcb->instructions;i++){
		printf("Comienzo linea de codigo: %d\n", pcb->indexCode[i].start);
		printf("Fin linea de codigo: %d\n", pcb->indexCode[i].offset);
	}
	for (int i=0;i<(dataProgram->cantidad_de_etiquetas+dataProgram->cantidad_de_funciones);i++){
		printf("Etiqueta: %s\n", pcb->indexTag[i].name);
		printf("Puntero de etiqueta: %d\n", pcb->indexTag[i].PC);
	}

	metadata_destruir(dataProgram);

}
