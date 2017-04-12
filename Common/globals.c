#include "globals.h"

#include <sys/stat.h>

static process_t current_process;

void set_current_process(process_t spid) {
	current_process = spid;
}

process_t get_current_process(void) {
	return current_process;
}

const char *get_process_name(process_t spid) {
	static char *process_names[] = {"kernel", "memoria", "fs", "cpu", "consola"};
	return process_names[spid];
}

const char *get_resource_path() {
	static char *resource_path = NULL;

	if(resource_path == NULL) {
		resource_path = string_duplicate(getenv("HOME"));
		string_append(&resource_path, "/tpsorsc/");
	}

	return resource_path;
}
