#include "console.h"
#include "utils.h"
#include "Memoria.h"

char command[BUFFER_CAPACITY];

void delay(string strvalue) {
	int intvalue;
	if(is_empty(strvalue)) {
		printf("%u ms\n", memory_get_access_time());
	} else if((intvalue = strtoi(strvalue)) != -1) {
		memory_set_access_time(intvalue);
	}
}

void help() {
	puts("dump [cache/structs/content] - muestra información sobre el contenido de la memoria");
	puts("size [<pid>]                 - muestra información sobre el tamaño de la memoria");
	puts("delay [<value>]              - cambia o muestra el tiempo de acceso a memoria");
	puts("flush                        - limpia el contenido de la memoria caché");
	puts("logout                       - libera toda la memoria y termina la ejecución");
	puts("help                         - muestra esta pantalla de ayuda");
}

void console() {
	title("Consola");

	while(true) {
		char *argument = input(command);
		if(streq(command, "logout")) return;

		if(streq(command, "help")) {
			help();
		} else if(streq(command, "dump")) {
			puts("[TODO]");
		} else if(streq(command, "delay")) {
			delay(argument);
		} else if(streq(command, "flush")) {
			memory_flush_cache();
			puts("Caché vaciada.");
		} else {
			puts("Comando no reconocido. Escriba 'help' para ayuda.");
		}
	}
}
