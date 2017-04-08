#include "utils.h"

typedef enum {Kernel, Memoria, FS, CPU, Consola} t_proceso;

void quit(const char *err) {
	if(err != NULL) {
		fflush(stdout);
		fprintf(stderr, "Error: %s\n", err);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

void guard(bool cond, const char *err) {
	if(cond) return;
	quit(err != NULL ? err : "guard error");
}
