#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>

typedef struct t_file_system{
	int puerto;
	char* punto_montaje;
}t_file_system;

int main(){

	t_config* config = config_create("./metadata");

	t_file_system* file_system = (t_file_system*) malloc(sizeof(t_file_system));

	file_system->puerto = config_get_int_value(config, "PUERTO");
	file_system->punto_montaje = config_get_string_value(config, "PUNTO_MONTAJE");

	printf("---------------Mi configuracion---------------\n");
	printf("PUERTO: %i\n", file_system->puerto);
	printf("PUNTO_MONTAJE: %s\n", file_system->punto_montaje);
	printf("----------------------------------------------\n");

	return 0;
}
