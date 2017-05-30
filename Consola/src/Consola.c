#include "Consola.h"
#include "utils.h"
#include "protocol.h"
#include "log.h"
#include "console.h"
#include "console_config.h"
#include "program_handler.h"
#include "message_receiver.h"

// Máximo de programas concurrentes para cada consola
#define MAX_THREADS 10

console_t console;

void start_program_thread(string path) {
	if(path == NULL) {
		puts("Debe ingresar una ruta al archivo.");
		return;
	}

	if(mlist_length(console.threads) >= MAX_THREADS) {
		puts("Demasiados programas en ejecución. Intente más tarde.");
		return;
	}

	program_t *program = malloc(sizeof(program_t));
	program->pid = -1;
	program->tid = thread_create(program_handler, path);
	program->sem = thread_sem(0);
	mlist_append(console.threads, program);
}

void init(string path) {
	title("Conexión");
	console.active = true;

	printf("Estableciendo conexión con el Kernel...");
	t_consola *config = console_config_create(path);
	string kernel_ip = console_config_kernel_ip(config);
	string kernel_port = console_config_kernel_port(config);
	console.kernel = socket_connect(kernel_ip, kernel_port);
	protocol_handshake_send(console.kernel);
	console_config_delete(config);
	printf("\33[2K\rConectado al Kernel en %s:%s\n", kernel_ip, kernel_port);

	console.threads = mlist_create();
	console.receiver = thread_create(message_receiver);
}

void terminate() {
	console.active = false;
	thread_kill(console.receiver);
	mlist_destroy(console.threads, destroy_program);
	socket_close(console.kernel);
	printf("\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CONSOLE);
	title("CONSOLA");

	init(argv[1]);
	start_console();
	terminate();
	return EXIT_SUCCESS;
}
