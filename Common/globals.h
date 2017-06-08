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

typedef enum {
	EXIT_PROGRAM_EXIT_SUCCESS = 0,
	EXIT_RESOURCES_UNAVAILABLE = -1,
	EXIT_FILE_NOT_FOUND = -2,
	EXIT_FILE_READ_FAILURE = -3,
	EXIT_FILE_WRITE_FAILURE = -4,
	EXIT_MEMORY_EXCEPTION = -5,
	EXIT_CONSOLE_DISCONNECTED = -6,
	EXIT_PROGRAM_KILLED = -7,
	EXIT_SEGMENT_LARGER_THAN_PAGE = -8,
	EXIT_PROCESS_REACHED_MAX_PAGES = -9,
	EXIT_UNDEFINED_ERROR = -20
} exit_code_t;

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
