#include "utils.h"
#include "globals.h"
#include "log.h"
#include <time.h>

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

void input(char *buffer) {
	printf("> ");
	fgets(buffer, BUFFER_CAPACITY, stdin);
	buffer[strcspn(buffer, "\n")] = '\0';
}

bool streq(const char *str1, const char *str2) {
	return string_equals_ignore_case((char*) str1, (char*) str2);
}

const char *datetime(time_t time) {
	struct tm lt;
	localtime_r(&time, &lt);
	return string_from_format("%d-%02d-%02d %02d:%02d:%02d", lt.tm_year + 1900, lt.tm_mon + 1,
			lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
}

const char *timediff(time_t t1, time_t t2) {
	unsigned duration = (unsigned) difftime(t1, t2);
	unsigned seconds = duration % 60;
	unsigned minutes = duration / 60;
	unsigned hours = minutes / 60;
	return string_from_format("%02u:%02u:%02u", hours, minutes, seconds);
}
