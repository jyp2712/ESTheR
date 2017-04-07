#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "Consola.h"

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");

	t_consola *consola = malloc(sizeof(t_consola));
	leerConfiguracionConsola(consola, argv[1]);

	printf("Conectandose al servidor...\n");
	int kernel_fd = socket_connect(consola->ip_kernel, consola->puerto_kernel);
	printf("Conectado al servidor. Ya puede enviar mensajes. Escriba 'exit' para salir\n");

//------------Envio de mensajes al servidor------------
	char message[SOCKET_BUFFER_CAPACITY];

	while(true) {
		fgets(message, SOCKET_BUFFER_CAPACITY, stdin);
		message[strcspn(message, "\n")] = '\0';
		if(!strcmp(message, "exit")) break;
		socket_send(message, kernel_fd);
	}

	free(consola);
	socket_close(kernel_fd);
	return 0;
}

void leerConfiguracionConsola(t_consola *consola, char *path) {

	t_config* config = config_create(path);

	consola->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	consola->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	printf("---------------Mi configuración---------------\n");
	printf("IP KERNEL: %s\n", consola->ip_kernel);
	printf("PUERTO KERNEL: %s\n", consola->puerto_kernel);
	printf("----------------------------------------------\n");
}

