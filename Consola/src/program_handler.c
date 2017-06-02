#include "program_handler.h"
#include <parser/sintax.h>
#include "protocol.h"
#include "log.h"

void notify_kernel(int pid) {
	header_t header = protocol_header(OP_PROGRAM_KILLED);
	header.usrpid = pid;
	packet_t packet = protocol_packet(header);
	protocol_packet_send(packet, console.kernel);
}

void destroy_program(program_t *program) {
	notify_kernel(program->pid);
	program->status = PROGRAM_KILLED;
	thread_sem_signal(&program->sem);
	thread_kill(program->tid);
	thread_sem_destroy(&program->sem);
	free(program);
}

void remove_program(program_t *program) {
	bool condition(program_t *element) {
		return element == program;
	}
	mlist_remove(console.programs, condition, destroy_program);
}

void program_handler(string path) {
	program_t *program = mlist_last(console.programs);

	char payload[PROGRAM_SIZE];
	ssize_t fsize = readfile(path, payload);
	if(fsize == -1) {
		printf("\33[2K\rError al leer el archivo. Intente nuevamente.\n> ");
		fflush(stdout);
		remove_program(program);
		return;
	}

	packet_t packet = protocol_packet(protocol_header(OP_NEW_PROGRAM, fsize), payload);
	protocol_packet_send(packet, console.kernel);

	unsigned num_prints = 0;
	time_t start_time = get_current_time();
	while(program->status == PROGRAM_ACTIVE) {
		thread_sem_wait(&program->sem);
		switch(program->status) {
		case PROGRAM_ACTIVE:
			print("[%03d] %s", program->pid, console.message);
			free(console.message);
			num_prints++;
			break;
		case PROGRAM_KILLED:
			log_inform("Program #%d killed", program->pid);
			if(console.active) {
				print("Programa #%d cancelado.", program->pid);
			}
			return;
		}
	}
	time_t end_time = get_current_time();

	log_inform("Program #%d ended", program->pid);
	print("Programa #%d terminado.", program->pid);
	print("Hora de inicio: %s", datetime(start_time));
	print("Hora de finalización: %s", datetime(end_time));
	print("Cantidad de impresiones: %u", num_prints);
	print("Tiempo de ejecución: %s", timediff(start_time, end_time));

	remove_program(program);
}
