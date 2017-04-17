#include "Consola.h"
#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "log.h"
#include "protocol.h"
#include "thread.h"
#include <time.h>

// Define el máximo de programas concurrentes para cada consola
// Por ahí podríamos ponerlo en el archivo de configuración
#define MAX_THREADS 10

thread_t threads[MAX_THREADS];
unsigned nthread = 0;

void show_command_list() {
	puts("run <ruta> - ejecuta el programa AnSISOP en la ruta especificada");
	puts("kill <pid> - termina el programa correspondiente al PID especificado");
	puts("logout     - termina todos los programas y desconecta la consola");
	puts("clear      - limpia todos los mensajes de la pantalla");
	puts("help       - muestra esta pantalla de ayuda");
}

void clear_screen() {
	system("printf '\033c'");
}

void *program_thread(void *path) {
	unsigned num_prints = 0;
	time_t start_time = time(NULL);
	while(true) {
		// TODO
	}
	time_t end_time = time(NULL);

	printf("Hora de inicio: %s\n", datetime(start_time));
	printf("Hora de finalización: %s\n", datetime(end_time));
	printf("Cantidad de impresiones: %u\n", num_prints);
	printf("Tiempo de ejecución: %s\n", timediff(start_time, end_time));
	return NULL;
}

/**
 * Crea un hilo de ejecución desde donde le manda la ruta al kernel para que empiece
 * a ejecutar el programa, y se queda a la espera de recibir mensajes para imprimir.
 * (Acá lo que no sé cómo vamos a hacer es cómo saber qué recv() tiene que agarrar
 * cada hilo, según el pid que corresponda. Igual para el segundo checkpoint no hace
 * falta porque va a haber un solo proceso así que lo veremos más adelante.)
 */
void run_program(const char *path) {
	if(nthread >= MAX_THREADS) return;
	thread_t thread = thread_create(program_thread, (void*) path);
	threads[nthread++] = thread;
}

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CONSOLE);

	t_consola *consola = malloc(sizeof(t_consola));
	leerConfiguracionConsola(consola, argv[1]);

	puts("Conectándose al Kernel...");
	int kernel_fd = socket_connect(consola->ip_kernel, consola->puerto_kernel);
	protocol_send_handshake(kernel_fd);
	puts("Conectado. Escriba 'help' para ver la lista de comandos disponibles.");

//------------Envio de mensajes al servidor------------
	char command[BUFFER_CAPACITY];

	while(true) {
		input(command);
		if(streq(command, "logout")) break;
		if(streq(command, "help")) {
			show_command_list();
		} else if(streq(command, "clear")) {
			clear_screen();
		} else if(string_starts_with(command, "run ")) {
			const char *path = command + 4;
			socket_send_string(path, kernel_fd);
//			run_program(path);
		}
	}

	free(consola);
//	socket_close(kernel_fd);
	return 0;
}

void leerConfiguracionConsola(t_consola *consola, char *path) {
	t_config* config = config_create(path);

	consola->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	consola->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	printf("---------------Mi configuración---------------\n");
	printf("IP KERNEL: %s\n", consola->ip_kernel);
	printf("PUERTO KERNEL: %s\n", consola->puerto_kernel);
	printf("----------------------------------------------\n");
}

