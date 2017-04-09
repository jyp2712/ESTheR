#include "globals.h"

#include <sys/stat.h>

static process_t process_type;

void set_process_type(process_t type) {
	process_type = type;
}

process_t get_process_type(void) {
	return process_type;
}

const char *get_process_name(void) {
	static char *process_names[] = {"kernel", "memoria", "fs", "cpu", "consola"};
	return process_names[process_type];
}

const char *get_resource_path() {
	static char *resource_path = NULL;

	if(resource_path == NULL) {
		resource_path = string_duplicate(getenv("HOME"));
		string_append(&resource_path, "/tpsorsc/");
	}

	return resource_path;
}
