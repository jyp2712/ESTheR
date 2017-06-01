#include "utils.h"
#include "Consola.h"
#include "program_handler.h"

char command[BUFFER_CAPACITY];

void show_command_list() {
	puts("run <ruta> - ejecuta el programa AnSISOP en la ruta especificada");
	puts("kill <pid> - termina el programa correspondiente al PID especificado");
	puts("list       - muestra una lista de todos los programas en ejecución");
	puts("logout     - termina todos los programas y desconecta la consola");
	puts("clear      - limpia todos los mensajes de la pantalla");
	puts("help       - muestra esta pantalla de ayuda");
}

void clear_screen() {
	system("printf '\033c'");
}

void list_programs() {
	int num_programs = mlist_length(console.programs);
	printf("Programas ejecutando: %d\n", num_programs);
	if(num_programs == 0) return;
	printf("PIDs: ");
	void routine(program_t *program) {
		printf("%d, ", program->pid);
	}
	mlist_traverse(console.programs, routine);
	printf("\b\b\33[K\n");
}

void kill_program(string argument) {
	if(argument == NULL) {
		puts("Debe ingresar un identificador de programa (PID)");
		return;
	}
	int pid = strtoi(argument);
	if(pid <= 0) {
		puts("PID inválido");
		return;
	}
	bool condition(program_t *element) {
		return element->pid == pid;
	}
	program_t *program = mlist_find(console.programs, condition);
	if(program == NULL) {
		puts("El PID ingresado no pertenece a ningún programa en ejecución");
		return;
	}
	remove_program(program);
}

void start_console() {
	title("Consola");
	while(true) {
		char *argument = input(command);
		if(streq(command, "logout")) break;
		if(streq(command, "list")) {
			list_programs();
		} else if(streq(command, "help")) {
			show_command_list();
		} else if(streq(command, "clear")) {
			clear_screen();
		} else if(streq(command, "run")) {
			start_program_thread(argument);
		} else if(streq(command, "kill")) {
			kill_program(argument);
		} else {
			puts("Comando no reconocido. Escriba 'help' para ayuda.");
		}
	}
}
