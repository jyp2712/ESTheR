#include "File_System.h"
#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(FS);

	t_file_system* file_system = malloc(sizeof(t_file_system));
	leerConfiguracionFileSystem(file_system, argv[1]);

	socket_t kernel_fd = socket_listen(file_system->puerto);

	process_t ptype = protocol_handshake_receive(kernel_fd);
	guard(ptype == KERNEL, "Unexpected handshake");
	puts("Recibido handshake del Kernel");

	char message[BUFFER_CAPACITY];

	while(socket_receive_string(message, kernel_fd) > 0) {
		printf("Recibido mensaje: \"%s\"\n", message);
	}

	socket_close(kernel_fd);
	free(file_system);

	return 0;
}

void leerConfiguracionFileSystem(t_file_system* file_system, char* path){

	t_config* config = config_create(path);

	file_system->puerto = config_get_string_value(config, "PUERTO");
	file_system->punto_montaje = config_get_string_value(config, "PUNTO_MONTAJE");

	printf("---------------Mi configuración---------------\n");
	printf("PUERTO: %s\n", file_system->puerto);
	printf("PUNTO_MONTAJE: %s\n", file_system->punto_montaje);
	printf("----------------------------------------------\n");
}
