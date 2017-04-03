#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include "File_System.h"

int main(){

	t_file_system* file_system = (t_file_system*) malloc(sizeof(t_file_system));

	leerConfiguracionFileSystem(file_system);

	free(file_system);

	return 0;
}

void leerConfiguracionFileSystem(t_file_system* file_system){

	t_config* config = config_create("../metadata");

	file_system->puerto = config_get_string_value(config, "PUERTO");
	file_system->punto_montaje = config_get_string_value(config, "PUNTO_MONTAJE");

	printf("---------------Mi configuracion---------------\n");
	printf("PUERTO: %s\n", file_system->puerto);
	printf("PUNTO_MONTAJE: %s\n", file_system->punto_montaje);
	printf("----------------------------------------------\n");
}
