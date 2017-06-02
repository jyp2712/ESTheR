#include "utils.h"
#include "globals.h"
#include "log.h"
#include <time.h>
#include <stdarg.h>

void quit(const char *err) {
	if(err != NULL) {
		printf("\n%s\n", err);
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

char *input(char *command) {
	printf("\33[2K\r> ");
	fgets(command, BUFFER_CAPACITY, stdin);
	command[strcspn(command, "\n")] = '\0';
	char *p = strchr(command, ' ');
	if(p != NULL) {
		*p = '\0';
		p++;
	}
	return p;
}

ssize_t readfile(const char *path, char *buffer) {
	if(!string_starts_with((char *) path, "/")) {
		char *rpath = string_duplicate((char *) get_resource_path());
		string_append(&rpath, "scripts/");
		string_append(&rpath, (char *) path);
		path = rpath;
	}

	FILE *file = fopen(path, "rb");
	if(file == NULL) {
		log_report("Couldn't open resource at %s", path);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);
	fread(buffer, fsize, 1, file);

	bool ok = ferror(file) == 0;
	fclose(file);
	buffer[fsize] = '\0';
	return ok ? fsize : -1;
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
	unsigned duration = abs((int) difftime(t1, t2));
	unsigned seconds = duration % 60;
	unsigned minutes = duration / 60;
	unsigned hours = minutes / 60;
	return string_from_format("%02u:%02u:%02u", hours, minutes, seconds);
}

void *alloc(size_t size) {
	void *p = calloc(1, size);
	guard(p != NULL, "Problemas reservando memoria");
	return p;
}

void title(string text) {
	printf("\n\33[1m\33[4m\33[33m%s\33[0m\n", text);
}

int strtoi(string text) {
	char *p;
	int res = (int) strtol(text, &p, 10);
	return *p == '\0' ? res : -1;
}

bool is_empty(string text) {
	if(text == NULL) return true;
	char *str = string_duplicate((char*) text);
	string_trim(&str);
	bool empty = string_is_empty(str);
	free(str);
	return empty;
}

inline time_t get_current_time() {
	return time(NULL);
}

void print(string format, ...) {
	va_list ap;
	va_start(ap, format);
	printf("\33[2K\r");
	vprintf(format, ap);
	printf("\n> ");
	fflush(stdout);
	va_end(ap);
}
