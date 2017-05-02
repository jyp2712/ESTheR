#ifndef globals_h
#define globals_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <commons/string.h>

#define BUFFER_CAPACITY 1024

typedef unsigned char byte;
typedef const char * string;

typedef enum {KERNEL, MEMORY, FS, CPU, CONSOLE} process_t;

/**
 * Establece el proceso en ejecución.
 * @param ID del proceso en ejecución.
 */
void set_current_process(process_t spid);

/**
 * Devuelve el proceso en ejecución.
 * @return ID del proceso en ejecución.
 */
process_t get_current_process(void);

/**
 * Devuelve una cadena con el nombre de un proceso determinado.
 * @param spid ID del proceso.
 * @return Nombre del proceso.
 */
const char *get_process_name(process_t spid);

/**
 * Devuelve la ruta al directorio de recursos (donde se guardan
 * los archivos de log y demás recursos que requiera el sistema).
 */
const char *get_resource_path();

#endif /* globals_h */
