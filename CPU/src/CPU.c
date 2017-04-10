#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "CPU.h"
#include <parser/metadata_program.h>

int main(int argc, char **argv) {
	set_process_type(CPU);

	guard(argc == 2, "Falta indicar ruta de archivo de configuración");

	t_cpu* cpu = malloc(sizeof(t_cpu));
	leerConfiguracionCPU(cpu, argv[1]);

	puts("Conectándose al Kernel...");
	int kernel_fd = socket_connect(cpu->ip_kernel, cpu->puerto_kernel);
	puts("Conectado.");

//------------Envio de mensajes al servidor------------
	char message[SOCKET_BUFFER_CAPACITY];

	while(socket_receive(message, kernel_fd) > 0) {
		printf ("Message received: \"%s\"\n", message);
	}

	free(cpu);
	socket_close(kernel_fd);
	return 0;
}

void leerConfiguracionCPU(t_cpu* cpu, char* path) {

	t_config* config = config_create(path);

	cpu->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	cpu->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	printf("---------------Mi configuración---------------\n");
	printf("IP KERNEL: %s\n", cpu->ip_kernel);
	printf("PUERTO KERNEL: %s\n", cpu->puerto_kernel);
	printf("----------------------------------------------\n");
}
