#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>

typedef struct t_consola{
	char* ip_kernel;
	int puerto_kernel;
}t_consola;

int main(){

	t_config* config = config_create("./metadata");

	t_consola* consola = (t_consola*) malloc(sizeof(t_consola));

	consola->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	consola->puerto_kernel = config_get_int_value(config, "PUERTO_KERNEL");

	printf("---------------Mi configuracion---------------\n");
	printf("IP KERNEL: %s\n", consola->ip_kernel);
	printf("PUERTO: %i\n", consola->puerto_kernel);
	printf("----------------------------------------------\n");

	return 0;

}
