#ifndef KERNEL_H_
#define KERNEL_H_

#define PACKAGESIZE 1024

#include <sys/inotify.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "socket.h"
#include "structures.h"
#include "protocol.h"
#include "utils.h"
#include <parser/parser.h>
#include <parser/metadata_program.h>
#include <parser/sintax.h>
#include <commons/collections/list.h>
#include "serial.h"
#include "thread.h"
#include "log.h"
#include <commons/config.h>
#include <commons/string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>

enum {NEW, READY, EXEC, BLOCK, EXIT};

typedef struct{
    char* puerto_prog;
    char* puerto_cpu;
    char* ip_memoria;
    char* puerto_memoria;
    char* ip_fs;
    char* puerto_fs;
    int quantum;
    int quantum_sleep;
    char* algoritmo;
    int grado_multiprog;
    char** sem_ids;
    char** sem_init;
    char** shared_vars;
    int* shared_values;
    int stack_size;
    int page_size;
}t_kernel;

typedef struct{
    socket_t clientID;
    unsigned char process;
    int status;
    int pid;
}t_client;

typedef struct{
	unsigned char* codigo;
	unsigned long size;
}t_code_ms;

char command[BUFFER_CAPACITY];
int generatorPid = 0;
pthread_mutex_t mutex_planificacion;
sem_t *sem_ansisop;

t_kernel* kernel;
t_list *pcb_ready, *pcb_new, *pcb_exec, *pcb_block, *pcb_exit;
t_list *consolas_conectadas, *consolas_desconectadas, *cpu_conectadas, *cpu_executing;
t_list *deadpid, *codes_ms;

void leerConfiguracionKernel(t_kernel*, char* path);

t_pcb* crear_pcb_proceso (t_metadata_program*);

void gestion_datos_newPcb(packet_t, t_client*);

void planificacion(socket_t);

t_client* buscar_proceso (socket_t);

#endif /* KERNEL_H_ */
