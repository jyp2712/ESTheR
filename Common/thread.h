#ifndef thread_h
#define thread_h

#include <pthread.h>

typedef pthread_t thread_t;

/**
 * Crea un hilo de usuario usando la librería de hilos POSIX.
 * @param routine Rutina que ejecutará el hilo.
 * @param arg Argumento pasado a la rutina.
 * @return Identificador del hilo creado.
 */
thread_t thread_create(void *(*routine)(void *), void *arg);

#endif /* thread_h */
