#include "utils.h"
#include "globals.h"
#include "log.h"

void quit(const char *err) {
	if(err != NULL) {
		printf("%s\n", err);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

void guard(bool cond, const char *err) {
	if(cond) return;
	quit(err != NULL ? err : "Guard error");
}

void fdcheck(long fd) {
	guard(fd != -1, strerror(errno));
}

void mkdirs(const char *path) {
	char *cmd = string_duplicate("mkdir -p ");
	string_append(&cmd, (char*) path);
	system(cmd);
	free(cmd);
}
