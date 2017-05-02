#include "Memoria.h"
#include <commons/config.h>
#include <commons/collections/list.h>
#include "log.h"
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "thread.h"
#include "Configuracion.h"
#include "operations.h"

#define CANT_CLIENTES 100

t_memoria *config;
thread_t thServidor;
socket_t skServidor;

char command[BUFFER_CAPACITY];

// Estado global de la Memoria
struct {
	byte *main; 	// Puntero a la memoria principal
	unsigned delay; // Retardo de acceso en milisegundos
} memory;

bool kernelConectado = false;

int main(int argc, char **argv) {
    set_current_process(MEMORY);
    title("MEMORIA");

	guard(argc == 2, "Falta indicar ruta de archivo de configuración");

	config = malloc(sizeof(t_memoria));
	leerConfiguracion(config, argv[1]);

	//inicializar memoria
	inicializar();

	//crear hilo servidor
	thServidor = thread_create(crearServidor);

	//crear interprete
	interpreteDeComandos();

	//liberar memoria y terminar
	terminate();
	return EXIT_SUCCESS;
}

void inicializar(){
	memory.main = alloc(config->marco_size * sizeof(byte));
	memory.delay = config->retardo_memoria;
}

void crearServidor(){
	thread_t thClientes[CANT_CLIENTES];
	int thClientesIndice = 0;

	socket_t skServidor = socket_init(NULL, config->puerto);
	socket_t cli_sock;
	//socket clientes en una lista o array dinamico
	while((cli_sock = socket_accept(skServidor)) != -1) {
		//crear thread hijo
		thClientes[thClientesIndice++] = thread_create(procesarCliente, &cli_sock);

		if(thClientesIndice >= CANT_CLIENTES) {
			quit("Demasiadas conexiones");
		}
	}

//	hacer join de los threads hijos
	for(int i = 0; i < thClientesIndice; i++) {
		thread_kill(thClientes[i]);
	}
}

void procesarCliente(void *arg) {
	socket_t sockfd = *(socket_t*)arg;
	process_t ptype = protocol_handshake_receive(sockfd);

	if(ptype == KERNEL) {
		if(kernelConectado){
			quitarConexion(sockfd, "Kernel already connected");
			return;
		}
		kernelConectado = true;
	}

	while(true) {
		packet_t packet = protocol_packet_receive(sockfd);
		if(packet.header.opcode == OP_UNDEFINED) {
			quitarConexion(sockfd, "Error on client connection");
			return;
		}

		switch(packet.header.opcode){
		case OP_ME_INIPRO:
			break;
		case OP_ME_SOLBYTPAG:
			break;
		case OP_ME_ALMBYTPAG:
			break;
		case OP_ME_ASIPAGPRO:
			break;
		case OP_ME_FINPRO:
			break;
		default:
			quitarConexion(sockfd, "Invalid operation");
			return;
		}
	}
}

void quitarConexion(socket_t sockfd, char *msg) {
	log_inform(msg);
	socket_close(sockfd);
}

void set_delay(string delaystr) {
	int delayint;
	if(delaystr == NULL) {
		printf("%u ms\n", memory.delay);
	} else if((delayint = strtoi(delaystr)) != -1) {
		memory.delay = delayint;
	}
}

void interpreteDeComandos() {
	title("Consola");
	while(true) {
		char *argument = input(command);
		if(streq(command, "fin")) return;

		if(streq(command, "?")) {
			printf("Comandos disponibles:\n");
			printf(" delay\n   Cambia o muestra el retardo de acceso a memoria\n");
			printf(" fin\n   Finaliza la terminal\n");
			printf(" dump\n   Dump de la memoria\n");
		} else if(streq(command, "delay")) {
			set_delay(argument);
		} else if(streq(command, "dump")) {
			printf("Dump...\n");
		} else {
			puts("Comando no reconocido. Escriba '?' para ayuda.");
		}
	}
}

void terminate() {
	thread_kill(thServidor);
	socket_close(skServidor);
	free(config);
	free(memory.main);
	puts("Proceso Memoria finalizado con éxito");
}
