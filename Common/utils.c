#include "utils.h"
#include "globals.h"
#include "log.h"

void quit(const char *err) {
	if(err != NULL) {
		log_report(err);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

void guard(bool cond, const char *err) {
	if(cond) return;
	quit(err != NULL ? err : "Guard error");
}

void mkdirs(const char *path) {
	char *cmd = string_duplicate("mkdir -p ");
	string_append(&cmd, (char*) path);
	system(cmd);
	free(cmd);
}
