#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "File_System.h"

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_process_type(FS);

	t_file_system* file_system = malloc(sizeof(t_file_system));
	leerConfiguracionFileSystem(file_system, argv[1]);

	int sockfd = socket_listen(file_system->puerto);

	char message[SOCKET_BUFFER_CAPACITY];

	while(socket_receive(message, sockfd) > 0) {
		printf("Message received: \"%s\"\n", message);
	}

	socket_close(sockfd);
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
