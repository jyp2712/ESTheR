#include "program_handler.h"
#include <parser/sintax.h>
#include "protocol.h"

void destroy_program(program_t *program) {
	thread_kill(program->tid);
	thread_sem_destroy(&program->sem);
	free(program);
}

void remove_program(program_t *program) {
	bool condition(program_t *element) {
		return element == program;
	}
	mlist_remove(console.threads, condition, destroy_program);
}

void program_handler(string path) {
	program_t *program = mlist_last(console.threads);

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
	while(true) {
		thread_sem_wait(&program->sem);
		printf("%s\n", console.message);
	}
	time_t end_time = get_current_time();

	printf("Hora de inicio: %s\n", datetime(start_time));
	printf("Hora de finalización: %s\n", datetime(end_time));
	printf("Cantidad de impresiones: %u\n", num_prints);
	printf("Tiempo de ejecución: %s\n", timediff(start_time, end_time));

	remove_program(program);
}
