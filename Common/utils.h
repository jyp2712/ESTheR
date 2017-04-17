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
 * un mensaje de error por el log y termina la ejecución del programa.
 * @param cond Condidición a verificar.
 * @param err Mensaje de error.
 */
void guard(bool cond, const char *err);

/**
 * Verifica el valor del descriptor de archivo. Si es -1, imprime un mensaje
 * de error por el log y termina la ejecución del programa.
 * @param fd Descriptor de archivo o número a verificar.
 */
void fdcheck(long fd);

/**
 * Crea todos los directorios que no existan en la ruta pasada por parámetro.
 * @param path Ruta al directorio a crear.
 */
void mkdirs(const char *path);

/**
 * Lee una línea de stdin y la guarda en buffer.
 * @param buffer Búfer donde se guardará la cadena leída.
 */
void input(char *buffer);

/**
 * Compara dos cadenas y devuelve un valor lógico indicando si son iguales.
 * No tiene en cuenta mayúsculas y minúsculas.
 * @param str1 Cadena 1.
 * @param str1 Cadena 2.
 * @return Valor indicando si la cadena 1 es igual a la cadena 2.
 */
bool streq(const char *str1, const char *str2);

/**
 * Devuelve una cadena con la representación ISO 8601 de un tiempo dado.
 * @param time Tiempo que se quiere convertir a ISO 8601.
 * @return Cadena representando un tiempo.
 */
const char *datetime(time_t time);

/**
 * Devuelve una cadena con una diferencia de tiempos expresada en hh:mm:ss.
 * @param t1 Tiempo inicial.
 * @param t2 Tiempo final.
 * @return Cadena con el intervalo de tiempo t2 - t1.
 */
const char *timediff(time_t t1, time_t t2);

#endif /* utils_h */
