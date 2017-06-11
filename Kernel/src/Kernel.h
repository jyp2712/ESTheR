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
#include "console.h"
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
#include "configuration.h"
#include "server.h"
#include "syscall.h"
#include "exitcode.h"

enum {NEW, READY, EXEC, BLOCK, EXIT};

typedef struct{
	unsigned char* codigo;
	unsigned long size;
}t_code_ms;

char command[BUFFER_CAPACITY];
pthread_mutex_t mutex_planificacion;

t_list *pcb_ready, *pcb_new, *pcb_exec, *pcb_block, *pcb_exit;
t_list *consolas_conectadas, *consolas_desconectadas, *cpu_conectadas, *cpu_executing;
t_list *deadpid, *codes_ms;

extern socket_t memfd;

t_pcb* crear_pcb_proceso (t_metadata_program*);

void gestion_datos_newPcb(packet_t, t_client*);

void planificacion(socket_t);

#endif /* KERNEL_H_ */
