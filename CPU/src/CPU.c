#include <commons/config.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "CPU.h"
#include "../../Common/sockets.h"

int main(int argc, char **argv){

	if(argc == 1){
		printf("Falta Indicar ruta de archivo de configuracion\n");
		abort();
	}

	char* path = argv[1];

	t_cpu* cpu = (t_cpu*) malloc(sizeof(t_cpu));

	leerConfiguracionCPU(cpu, path);

	int serverKernel;

	printf("Conectandose al servidor...\n");
	create_socketClient(&serverKernel, cpu->ip_kernel, cpu->puerto_kernel);
	printf("Conectado al servidor. Ya puede enviar mensajes. Escriba 'exit' para salir\n");

//------------Envio de mensajes al servidor------------
	int enviar = 1;
	char message[PACKAGESIZE];

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message,"exit\n")) enviar = 0;
		if (enviar) send(serverKernel, message, strlen(message) + 1, 0);
	}

	free(cpu);
	close(serverKernel);
	return 0;

	return 0;
}

void leerConfiguracionCPU(t_cpu* cpu, char* path){

	t_config* config = config_create(path);

	cpu->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	cpu->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	printf("---------------Mi configuracion---------------\n");
	printf("IP KERNEL: %s\n", cpu->ip_kernel);
	printf("PUERTO KERNEL: %s\n", cpu->puerto_kernel);
	printf("----------------------------------------------\n");
}
