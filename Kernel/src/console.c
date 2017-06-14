#include "console.h"
#include "Kernel.h"

char command[BUFFER_CAPACITY];

void show_help() {
	puts("list [<status>] - muestra todos los programas en un determinado estado");
	puts("info <pid>      - muestra información del programa asociado al PID ingresado");
	puts("kill <pid>      - finaliza el programa correspondiente al PID ingresado");
	puts("multipg <deg>   - cambia del grado de multiprogramación del sistema");
	puts("stop            - detiene la planificación de los procesos");
	puts("restart         - inicia la planificación de los procesos detenida");
	puts("help            - muestra esta pantalla de ayuda");
}

void list_status_queues(string name) {
	int status = -1;
	if(!is_empty(name)) {
		status = pcb_status_from_name(name);
	}
	if(status != -1) {
		pcb_print_status_queue(status);
	} else {
		pcb_print_status_queue(NEW);
		pcb_print_status_queue(READY);
		pcb_print_status_queue(EXEC);
		pcb_print_status_queue(BLOCK);
		pcb_print_status_queue(EXIT);
	}
}

void show_info(string arg) {
	int pid = -1;
	t_pcb *pcb = NULL;
	if(is_empty(arg)
			|| (pid = strtoi(arg)) == -1
			|| (pcb = pcb_by_id(pid)) == NULL) {
		puts("Debe ingresar un PID asociado a un programa manejado por el Kernel.");
		return;
	}
	puts("[TODO]");
}

void set_multiprog_level(string arg) {
	if(is_empty(arg)) {
		printf("%d\n", multipg_level);
		return;
	}
	int level = strtoi(arg);
	if(level < 0) { puts("Valor inválido."); return; }
	multipg_level = level;
}

void terminal() {
	title("Consola");

	while(true) {
		string argument = input(command);
		if(streq(command, "processlist")) {
			for (int i = 0; i < mlist_length(pcb_exec); i++){
				t_pcb* aux = mlist_get(pcb_exec, i);
				printf("PID: %d\n", aux->idProcess);
			}
			for (int i = 0; i < mlist_length(pcb_exit); i++){
				t_pcb* aux = mlist_get(pcb_exit, i);
				printf("PID: %d\n", aux->idProcess);
			}
			for (int i = 0; i < mlist_length(pcb_new); i++){
				t_pcb* aux = mlist_get(pcb_new, i);
				printf("PID: %d\n", aux->idProcess);
			}
			for (int i = 0; i < mlist_length(pcb_block); i++){
				t_pcb* aux = mlist_get(pcb_block, i);
				printf("PID: %d\n", aux->idProcess);
			}
			for (int i = 0; i < mlist_length(pcb_ready); i++){
				t_pcb* aux = mlist_get(pcb_ready, i);
				printf("PID: %d\n", aux->idProcess);
			}
		} else if(streq(command, "kill")) {
			int pid = atoi(argument);
			bool stop = 0;
			while (!stop){
				for (int i = 0; i < mlist_length(pcb_new); i++){
					t_pcb* aux = mlist_get(pcb_new, i);
					if (aux->idProcess == pid){
						aux = mlist_pop(pcb_new, i);
						aux->exitCode = -2;
						aux->status = EXIT;
						mlist_append(pcb_exit, aux);
						stop = 1;
						break;
					}
				}
				for (int i = 0; i < mlist_length(pcb_block); i++){
					t_pcb* aux = mlist_get(pcb_block, i);
					if (aux->idProcess == pid){
						aux = mlist_pop(pcb_block, i);
						aux->exitCode = -2;
						aux->status = EXIT;
						mlist_append(pcb_exit, aux);
						stop = 1;
						break;
					}
				}
				for (int i = 0; i < mlist_length(pcb_ready); i++){
					t_pcb* aux = mlist_get(pcb_ready, i);
					if (aux->idProcess == pid){
						aux = mlist_pop(pcb_ready, i);
						aux->exitCode = -2;
						aux->status = EXIT;
						mlist_append(pcb_exit, aux);
						stop = 1;
						break;
					}
				}
			}
		} else if(streq(command, "stop")) {
			pthread_mutex_lock(&mutex_planificacion);
		} else if(streq(command, "restart")) {
			pthread_mutex_unlock(&mutex_planificacion);
		} else if(streq(command, "list")) {
			list_status_queues(argument);
		} else if(streq(command, "info")) {
			show_info(argument);
		} else if(streq(command, "multipg")) {
			set_multiprog_level(argument);
		} else if(streq(command, "help")) {
			show_help();
		} else {
			puts("Comando no reconocido. Escriba 'help' para ayuda.");
		}
	}
}
