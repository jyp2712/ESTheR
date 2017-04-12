#include "log.h"

#include <commons/string.h>
#include <commons/log.h>

#define LOG_BUFFER_SIZE 256

static const char *log_path(void) {
	static char *path = NULL;

	if(path == NULL) {
		path = string_new();
		string_append(&path, (char*) get_resource_path());
		string_append(&path, "logs/");
		mkdirs(path);
		string_append(&path, (char*) get_process_name(get_current_process()));
		string_append(&path, ".txt");
	}

	return path;
}

static t_log *get_logger(void) {
	static t_log *logger = NULL;

	if(logger == NULL) {
		logger = log_create((char*) log_path(),
				(char*) get_process_name(get_current_process()), LOG_STDOUT, LOG_LEVEL_TRACE);
	}

	return logger;
}

static void log_template(bool error, const char *format, va_list args) {
	if(!LOG_ENABLED) return;

	char message[LOG_BUFFER_SIZE];

	vsnprintf(message, LOG_BUFFER_SIZE, format, args);

	if(error) {
		log_error(get_logger(), message);
	} else {
		log_debug(get_logger(), message);
	}
}

void log_inform(const char *format, ...) {
	va_list args;
	va_start(args, format);
	log_template(false, format, args);
	va_end(args);
}

void log_report(const char *format, ...) {
	va_list args;
	va_start(args, format);
	log_template(true, format, args);
	va_end(args);
}
