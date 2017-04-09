#ifndef utils_h
#define utils_h

#include "globals.h"

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

/**
 * Crea todos los directorios que no existan en la ruta pasada por parámetro.
 * @param path Ruta al directorio a crear.
 */
void mkdirs(const char *path);

#endif /* utils_h */
