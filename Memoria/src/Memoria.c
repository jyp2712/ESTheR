#include "Memoria.h"
#include <commons/config.h>
#include <commons/collections/list.h>
#include "log.h"
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "thread.h"
#include "Configuracion.h"
#include "serial.h"

#define CANT_CLIENTES 100

t_memoria *config;
thread_t thServidor;
socket_t skServidor;

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

		header_t header;
		int tamanioPag = 10; //Harcodeada, deberiamos levantarla del archivo de config, no entendi si era el MARCO_SIZE u otra cosa
		unsigned char buff[BUFFER_CAPACITY];

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
		case OP_CPU_TAMPAG_REQUEST:
			header = protocol_header (OP_CPU_TAMPAG_REQUEST);
			header.msgsize = serial_pack(buff, "h", tamanioPag);
			packet_t packet = protocol_packet (header, buff);
			protocol_packet_send(packet, sockfd);
			break;
		case OP_CPU_PROX_INST_REQUEST:
			header = protocol_header (OP_CPU_PROX_INST_REQUEST);
			header.msgsize = serial_pack(buff, "s", buscarProximaInstruccion());
			packet_t packet2 = protocol_packet (header, buff);
			protocol_packet_send(packet2, sockfd);
			break;
		default:
			quitarConexion(sockfd, "Invalid operation");
			return;
		}
	}
}

char* buscarProximaInstruccion(){

	//TODO metodo de busqueda de proxima instruccion

	return "variables a, b";
}

void quitarConexion(socket_t sockfd, char *msg) {
	log_inform(msg);
	socket_close(sockfd);
}

void interpreteDeComandos() {
	char comando[80];

	title("Consola");
	while(true) {
		input(comando);

		if(streq(comando, "?")) {
			printf("Comandos disponibles:\n");
			printf(" fin\n   Finaliza la terminal\n");
			printf(" dump\n   Dump de la memoria\n");
		} else if(streq(comando, "fin")) {
			return;
		} else if(streq(comando, "dump")) {
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
	puts("Proceso Memoria finalizado con éxito");
}
