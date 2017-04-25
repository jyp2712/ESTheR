#include "Memoria.h"
#include <unistd.h>
#include <signal.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "Configuracion.h"

#define CANT_CLIENTES 100

pthread_t thServidor;
socket_t skServidor;

int finServidor = 0;

int main(int argc, char **argv) {
    set_current_process(MEMORY);

	guard(argc == 2, "Falta indicar ruta de archivo de configuración");

	t_memoria* config = malloc(sizeof(t_memoria));
	leerConfiguracion(config, argv[1]);
	//leerConfiguracion(config, "metadata");

	//inicializar memoria
	inicializar(config);

	//crear servidor
	guard(pthread_create(&thServidor, NULL, crearServidor, config) == 0, "No se pudo crear hilo del servidor");

	//crear interprete
	interpreteDeComandos(config);

	//liberar memoria
	free(config);

	return 0;
}



void inicializar(t_memoria* config){

}

void *atenderSenial(int sig, siginfo_t *info, void *ucontext)
{
	finServidor = 1;
	return NULL;
}

void *crearServidor(t_memoria* config){
	pthread_t thClientes[CANT_CLIENTES];
	int thClientesIndice = 0;

	struct sigaction sa;
	sa.sa_handler = NULL;
	sa.sa_sigaction = atenderSenial;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	guard(sigaction(SIGUSR1, &sa, NULL) == 0, "No se pudo crear la señal");

	socket_t skServidor = socket_init(NULL, config->puerto);
	//socket clientes en una lista o array dinamico
	while(!finServidor) {
		socket_t cli_sock = socket_accept_v2(skServidor);

		if(cli_sock != -1) {
			//crear thread hijo
			if(pthread_create(&thClientes[thClientesIndice++], NULL, procesarCliente, &cli_sock)){
				printf("No se pudo crear hilo del cliente");
			}
			//considerar que si no se acepta un cliente cerrar la conexión
		}
		if(thClientesIndice >= CANT_CLIENTES) {
			printf("Te sarpaste en conexiones\n");
			return NULL;
		}
	}
	//hacer join de los threads hijos
	int i;
	for(i = 0; i < thClientesIndice; i++)
	{
		pthread_kill(thClientes[i], SIGUSR1);
		pthread_join(thClientes[i], NULL);
	}

	return NULL;
}

void *procesarCliente(socket_t *sockfd) {
	header_t header;
	if(validarHandshake(*sockfd, &header) == -1) {
		quitarConexion(*sockfd, "No valida Handshake");
		return NULL;
	}

	while(1) {
		// handle data from a client
		packet_t packet = protocol_packet_receive(*sockfd);
		if(packet.header.opcode == OP_UNDEFINED) {
			quitarConexion(*sockfd, "Ocurrió un error o se cerró la conexión desde el cliente");
			return NULL;
		}

		switch(header.opcode){
		case 1:
			break;
		default:
			quitarConexion(*sockfd, "Operación inválida");
			return NULL;
		}
	}

	return NULL;
}

void quitarConexion(socket_t sockfd, char *msg) {
	log_inform(msg);
	close(sockfd);
	log_inform("Se cerró la conexión %d", sockfd);
}

int validarHandshake(socket_t sockfd, header_t *header) {
	header_t h;
	if((header->syspid == KERNEL && header->opcode == OP_HANDSHAKE)
			|| (header->syspid == CPU && header->opcode == OP_HANDSHAKE)) {
		h.syspid = 0;
		h.opcode = 0;
		h.msgsize = 0;
		protocol_packet_send(protocol_packet(h), sockfd);

		return 0;
	}
	h.syspid = 0;
	h.opcode = 1;
	h.msgsize = 0;
	protocol_packet_send(protocol_packet(h), sockfd);

	return -1;
}

void interpreteDeComandos(t_memoria* config){
	int finConsola = 0;
	char comando[80];

	printf("Escriba ? para ayuda de comandos disponibles\n");
	do {
		printf ("Comando: ");
		scanf ("%s", comando);

		if(strcmp(comando, "?") == 0) {
			printf("Comandos disponibles:\n");
			printf(" fin\n   Finaliza la terminal\n");
			printf(" dump\n   Dump de la memoria\n");
		}
		else if(strcmp(comando, "fin") == 0) {
			//terminar servidor
			pthread_kill(thServidor, SIGUSR1);

			pthread_join(thServidor, NULL);
			finConsola = 1;

			printf("Proceso Memoria finalizado con éxito\n");
		}
		else if(strcmp(comando, "dump") == 0) {
			printf("Dump...\n");
		}
	} while(!finConsola);
}
