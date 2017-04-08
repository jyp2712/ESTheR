#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>

typedef enum {Kernel, Memoria, FS, CPU, Consola} t_proceso;


/**
 * Termina el programa normalmente si no se le pasa ningún mensaje de error.
 * De lo contrario, imprime el error por stderr y termina el programa con
 * código de salida 1 (error).
 * @param err Mensaje de error.
 */
void quit(const char *err);

/**
 * Verifica el valor booleano de una condición. En caso de ser falso, imprime
 * un mensaje de error por stderr y termina la ejecución del programa.
 * @param cond Condidición a verificar.
 * @param err Mensaje de error.
 */
void guard(bool cond, const char *err);

#endif /* utils_h */
