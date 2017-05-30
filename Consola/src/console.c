#include "utils.h"
#include "Consola.h"

char command[BUFFER_CAPACITY];

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

void start_console() {
	title("Consola");
	while(true) {
		char *argument = input(command);
		if(streq(command, "logout")) break;

		if(streq(command, "debug")) {
			int i = 0;
			printf("Programs running: %d\n", mlist_length(console.threads));
			void routine(program_t *program) {
				printf(" Program %d: PID=%d; TID=%ld\n", i++, program->pid, program->tid);
			}
			mlist_traverse(console.threads, routine);
			continue;
		}

		if(streq(command, "help")) {
			show_command_list();
		} else if(streq(command, "clear")) {
			clear_screen();
		} else if(streq(command, "run")) {
			start_program_thread(argument);
		} else {
			puts("Comando no reconocido. Escriba 'help' para ayuda.");
		}
	}
}
