#ifndef CONSOLE_CONFIG_H_
#define CONSOLE_CONFIG_H_

#include "utils.h"

typedef struct config t_consola;

t_consola *console_config_create(const char *path);

string console_config_kernel_ip(t_consola *config);

string console_config_kernel_port(t_consola *config);

void console_config_delete(t_consola *config);

#endif /* CONSOLE_CONFIG_H_ */
