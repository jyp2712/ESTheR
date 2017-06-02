/*
 * Consola.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "socket.h"
#include "thread.h"
#include "mlist.h"

enum program_status {PROGRAM_ACTIVE, PROGRAM_ENDED, PROGRAM_KILLED};

typedef struct {
	socket_t kernel;
	mlist_t *programs;
	char *message;
	bool active;
	thread_t receiver;
} console_t;

extern console_t console;

typedef struct {
	int pid;
	thread_t tid;
	sem_t sem;
	int status;
} program_t;

void start_program_thread(string path);

void terminate();

#endif /* CONSOLA_H_ */
