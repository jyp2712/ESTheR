#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define PACKAGESIZE 1024

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
}t_consola;

void leerConfiguracionConsola(t_consola* consola);

int main(){

	t_consola* consola = (t_consola*) malloc(sizeof(t_consola));

	leerConfiguracionConsola(consola);

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(consola->ip_kernel, consola->puerto_kernel, &hints, &serverInfo);

	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	int enviar = 1;
	char message[PACKAGESIZE];

	printf("Conectado al servidor. Ya puede enviar mensajes. Escriba 'exit' para salir\n");

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);			// Lee una linea en el stdin hasta encontrar un \n (y lo incluye) o llegar a PACKAGESIZE.
		if (!strcmp(message,"exit\n")) enviar = 0;	// Chequeo que el usuario no quiera salir
		if (enviar) send(serverSocket, message, strlen(message) + 1, 0); 	// Solo envio si el usuario no quiere salir.
	}

	close(serverSocket);

	free(consola);

	return 0;
}

void leerConfiguracionConsola(t_consola* consola){

	t_config* config = config_create("../metadata");

	consola->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	consola->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	printf("---------------Mi configuracion---------------\n");
	printf("IP KERNEL: %s\n", consola->ip_kernel);
	printf("PUERTO KERNEL: %s\n", consola->puerto_kernel);
	printf("----------------------------------------------\n");
}

