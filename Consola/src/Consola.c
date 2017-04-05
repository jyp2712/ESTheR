#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include "Consola.h"
#include "../../Common/sockets.h"


int main(int argc, char **argv){

	if(argc == 1){
		printf("Falta Indicar ruta de archivo de configuracion\n");
		abort();
	}

	char* path = argv[1];

	t_consola* consola = (t_consola*) malloc(sizeof(t_consola));

	leerConfiguracionConsola(consola, path);

	int serverKernel;

	printf("Conectandose al servidor...\n");
	create_socketClient(&serverKernel, consola->ip_kernel, consola->puerto_kernel);
	printf("Conectado al servidor. Ya puede enviar mensajes. Escriba 'exit' para salir\n");

//------------Envio de mensajes al servidor------------
	int enviar = 1;
	char message[PACKAGESIZE];

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message,"exit\n")) enviar = 0;
		if (enviar) send(serverKernel, message, strlen(message) + 1, 0);
	}

	free(consola);
	close(serverKernel);
	return 0;
}

void create_serverSocket(int* listenningSocket, char* port){

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(NULL, port, &hints, &serverInfo);
	*listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	bind(*listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);

	freeaddrinfo(serverInfo);

	listen(*listenningSocket, BACKLOG);
}

void accept_connection(int listenningSocket, int* clientSocket){

	struct sockaddr_in addr;

	socklen_t addrlen = sizeof(addr);

	*clientSocket = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);
}

void leerConfiguracionConsola(t_consola* consola, char* path){

	t_config* config = config_create(path);

	consola->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	consola->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	printf("---------------Mi configuracion---------------\n");
	printf("IP KERNEL: %s\n", consola->ip_kernel);
	printf("PUERTO KERNEL: %s\n", consola->puerto_kernel);
	printf("----------------------------------------------\n");
}

