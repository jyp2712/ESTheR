#ifndef macros_h
#define macros_h

#include <commons/temporal.h>
#include <commons/process.h>

#define typefmt(x) \
		(__builtin_types_compatible_p(typeof(x), char[]) ? "%s" : \
		__builtin_types_compatible_p(typeof(x), const char*) ? "%s" : \
		__builtin_types_compatible_p(typeof(x), char*) ? "%s" : \
		__builtin_types_compatible_p(typeof(x), double) ? "%f" : \
		__builtin_types_compatible_p(typeof(x), float) ? "%f" : \
		__builtin_types_compatible_p(typeof(x), long) ? "%ld" : \
		__builtin_types_compatible_p(typeof(x), int) ? "%d" : \
		__builtin_types_compatible_p(typeof(x), unsigned) ? "%u" : \
		__builtin_types_compatible_p(typeof(x), short) ? "%hd" : \
		__builtin_types_compatible_p(typeof(x), unsigned char) ? "%hhu" : \
		__builtin_types_compatible_p(typeof(x), void*) ? "%p" : \
		__builtin_types_compatible_p(typeof(x), char) ? "%c" : "void")

#define typestr(x) \
		(__builtin_types_compatible_p(typeof(x), char[]) ? "string" : \
		__builtin_types_compatible_p(typeof(x), const char*) ? "string" : \
		__builtin_types_compatible_p(typeof(x), char*) ? "string" : \
		__builtin_types_compatible_p(typeof(x), double) ? "double" : \
		__builtin_types_compatible_p(typeof(x), float) ? "float" : \
		__builtin_types_compatible_p(typeof(x), long) ? "long" : \
		__builtin_types_compatible_p(typeof(x), int) ? "int" : \
		__builtin_types_compatible_p(typeof(x), unsigned) ? "unsigned" : \
		__builtin_types_compatible_p(typeof(x), short) ? "short" : \
		__builtin_types_compatible_p(typeof(x), unsigned char) ? "byte" : \
		__builtin_types_compatible_p(typeof(x), void*) ? "pointer" : \
		__builtin_types_compatible_p(typeof(x), char) ? "char" : "void")

#define debug(x) \
		{ const char *_tstr = typestr(x); \
		bool _isstr = !strcmp(_tstr, "string"), _ischr = !strcmp(_tstr, "char"); \
		char *_time = temporal_get_string_time(); \
		const char *_proc = get_process_name(get_current_process()); \
		unsigned _pid = process_getpid(), _tid = process_get_thread_id(); \
		printf("\33[1m\33[36m[DEBUG] %s %s/(%u:%u): ", _time, _proc, _pid, _tid); \
		free(_time); \
		printf("%s " #x " = ", _tstr); \
		printf(_ischr ? "'" : _isstr ? "\"" : ""); \
		printf(typefmt(x), x); \
		printf(_ischr ? "'" : _isstr ? "\"" : ""); \
		printf("\33[0m\n"); }

#endif /* macros_h */
