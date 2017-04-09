#ifndef globals_h
#define globals_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <commons/string.h>

typedef enum {KERNEL, MEMORY, FS, CPU, CONSOLE} process_t;

/**
 * Establece el proceso en ejecución.
 * @return Nombre del proceso en ejecución.
 */
void set_process_type(process_t type);
process_t get_process_type(void);

/**
 * Devuelve una cadena con el nombre del proceso en ejecución.
 * @return Nombre del proceso en ejecución.
 */
const char *get_process_name(void);

/**
 * Devuelve la ruta al directorio de recursos (donde se guardan
 * los archivos de log y demás recursos que requiera el sistema).
 */
const char *get_resource_path();

#endif /* globals_h */
