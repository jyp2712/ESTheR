#include "console_config.h"
#include <commons/config.h>

struct config {
	char *ip_kernel;
	char *puerto_kernel;
};

t_consola *console_config_create(const char *path) {
	t_consola *config = malloc(sizeof(t_consola));
	t_config* conf = config_create((char*) path);

	config->ip_kernel = config_get_string_value(conf, "IP_KERNEL");
	config->puerto_kernel = config_get_string_value(conf, "PUERTO_KERNEL");

	title("Configuración");
	printf("IP KERNEL: %s\n", config->ip_kernel);
	printf("PUERTO KERNEL: %s\n", config->puerto_kernel);

	return config;
}

string console_config_kernel_ip(t_consola *config) {
	return config->ip_kernel;
}

string console_config_kernel_port(t_consola *config) {
	return config->puerto_kernel;
}

void console_config_delete(t_consola *config) {
	free(config);
}
