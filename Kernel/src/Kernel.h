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
#include "mlist.h"
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
#include "pcbutils.h"

enum {NEW, READY, EXEC, BLOCK, EXIT};

typedef struct{
	unsigned char* codigo;
	unsigned long size;
}t_code_ms;

pthread_mutex_t mutex_planificacion;

mlist_t *pcb_ready, *pcb_new, *pcb_exec, *pcb_block, *pcb_exit;
mlist_t *consolas_conectadas, *cpu_conectadas, *cpu_executing;
mlist_t *deadpid, *codes_ms;

extern socket_t memfd;
extern int multipg_level;

void init(void);
void terminate(void);

void create_connections(void);

t_pcb* crear_pcb_proceso (t_metadata_program*);

void gestion_datos_newPcb(packet_t, t_client*);

void planificacion(socket_t);

#endif /* KERNEL_H_ */
