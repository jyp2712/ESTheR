#include "utils.h"

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


t_log* crearArchivoLog(char* nombreProceso) {

	char* path = string_new();
	string_append(&path, "log");
	string_append(&path, nombreProceso);

	remove(path);

	t_log* logs = log_create(path, nombreProceso, 0, LOG_LEVEL_TRACE);
	if (logs == NULL) {
		puts("No se pudo generar el archivo de logueo.\n");
		return NULL;
	}

	log_info(logs, "ARCHIVO DE LOGUEO INICIALIZADO");

	return logs;
}
