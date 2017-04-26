#include "Memoria.h"
#include <unistd.h>
#include <signal.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include "log.h"
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "Configuracion.h"

#define CANT_CLIENTES 100

pthread_t thServidor;
socket_t skServidor;

int finServidor = 0;
int kernelConectado = 0;

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

	guard(sigaction(SIGUSR1, &sa, NULL) == 0, "Can't create signal user");

	socket_t skServidor = socket_init(NULL, config->puerto);
	//socket clientes en una lista o array dinamico
	while(!finServidor) {
		socket_t cli_sock = socket_accept_v2(skServidor);

		if(cli_sock != -1) {
			//crear thread hijo
			if(pthread_create(&thClientes[thClientesIndice++], NULL, procesarCliente, &cli_sock)){
				log_report("Can't create child thread for client connection");
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

void *procesarCliente(void *arg) {
	socket_t sockfd = *(socket_t*)arg;
	process_t ptype = protocol_handshake_receive(sockfd);

	if(ptype == KERNEL){
		if(kernelConectado){
			quitarConexion(sockfd, "Kernel already connected");
			return NULL;
		}
		else {
			kernelConectado = 1;
		}
	}

	while(1) {
		packet_t packet = protocol_packet_receive(sockfd);
		if(packet.header.opcode == OP_UNDEFINED) {
			quitarConexion(sockfd, "Error on client connection");
			return NULL;
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
			return NULL;
		}
	}

	return NULL;
}

void quitarConexion(socket_t sockfd, char *msg) {
	log_inform(msg);
	close(sockfd);
	log_inform("The socket %d has closed", sockfd);
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
