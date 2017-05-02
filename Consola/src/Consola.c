#include "Consola.h"
#include <commons/config.h>
#include <parser/sintax.h>
#include "utils.h"
#include "socket.h"
#include "log.h"
#include "protocol.h"
#include "thread.h"

// Define el máximo de programas concurrentes para cada consola
// Por ahí podríamos ponerlo en el archivo de configuración
#define MAX_THREADS 10

t_consola *config;
socket_t kernel_fd;
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

void connect_to_kernel() {
	title("Conexión");
	printf("Estableciendo conexión con el Kernel...");
	kernel_fd = socket_connect(config->ip_kernel, config->puerto_kernel);
	protocol_handshake_send(kernel_fd);
	printf("\33[2K\rConectado al Kernel en %s:%s\n", config->ip_kernel, config->puerto_kernel);
}

/**
 * Le manda el archivo al kernel para que empiece a ejecutar el programa,
 * y se queda a la espera de recibir mensajes para imprimir.
 * (Acá lo que no sé cómo vamos a hacer es cómo saber qué recv() tiene que agarrar
 * cada hilo, según el pid que corresponda. Igual para el segundo checkpoint no hace
 * falta porque va a haber un solo proceso así que lo veremos más adelante.)
 */
void run_program(const char *path) {
	char payload[PROGRAM_SIZE];
	ssize_t fsize = readfile(path, payload);
	if(fsize == -1) {
		printf("\33[2K\rError al leer el archivo. Intente nuevamente.\n> ");
		fflush(stdout);
		return;
	}

	packet_t packet = protocol_packet(protocol_header(OP_NEW_PROGRAM, fsize), payload);
	protocol_packet_send(packet, kernel_fd);

	unsigned num_prints = 0;
	time_t start_time = time(NULL);
	while(true) {
		// TODO: lógica de recepción de mensajes para el programa
	}
	time_t end_time = time(NULL);

	printf("Hora de inicio: %s\n", datetime(start_time));
	printf("Hora de finalización: %s\n", datetime(end_time));
	printf("Cantidad de impresiones: %u\n", num_prints);
	printf("Tiempo de ejecución: %s\n", timediff(start_time, end_time));
}

void start_program_thread(const char *path) {
	if(*path == '\0') {
		puts("Debe ingresar una ruta al archivo.");
		return;
	}

	if(nthread >= MAX_THREADS) {
		puts("Demasiados programas en ejecución. Intente más tarde.");
		return;
	}

	threads[nthread++] = thread_create(run_program, path);
}

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CONSOLE);
	title("CONSOLA");

	leerConfiguracionConsola(argv[1]);

	connect_to_kernel();

//------------Envio de mensajes al servidor------------
	char command[BUFFER_CAPACITY] = {0};

	title("Consola");
	while(true) {
		input(command);
		if(streq(command, "logout")) break;
		if(streq(command, "help")) {
			show_command_list();
		} else if(streq(command, "clear")) {
			clear_screen();
		} else if(string_starts_with(command, "run")) {
			start_program_thread(command + 4);
		} else {
			puts("Comando no reconocido. Escriba 'help' para ayuda.");
		}
	}

	socket_close(kernel_fd);
	free(config);
	return 0;
}

void leerConfiguracionConsola(const char *path) {
	config = malloc(sizeof(t_consola));
	t_config* conf = config_create(path);

	config->ip_kernel = config_get_string_value(conf, "IP_KERNEL");
	config->puerto_kernel = config_get_string_value(conf, "PUERTO_KERNEL");

	title("Configuración");
	printf("IP KERNEL: %s\n", config->ip_kernel);
	printf("PUERTO KERNEL: %s\n", config->puerto_kernel);
}
