#ifndef macros_h
#define macros_h

#include <commons/temporal.h>
#include <commons/process.h>

#define DEFAULTS_1(fn, a, ...) fn(a)
#define DEFAULTS_2(fn, a, b, ...) fn(a, b)
#define DEFAULTS_3(fn, a, b, c, ...) fn(a, b, c)
#define DEFAULTS_4(fn, a, b, c, d, ...) fn(a, b, c, d)
#define DEFAULTS_5(fn, a, b, c, d, e, ...) fn(a, b, c, d, e)
#define DEFAULTS_6(fn, a, b, c, d, e, f, ...) fn(a, b, c, d, e, f)
#define DEFAULTS_7(fn, a, b, c, d, e, f, g, ...) fn(a, b, c, d, e, f, g)
#define DEFAULTS(f, n, ...) DEFAULTS_##n(f, __VA_ARGS__)

#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...)  EVAL4(EVAL4(EVAL4(__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT
#define MAP_COMMA ,

#define MAP_GET_END2() 0, MAP_END
#define MAP_GET_END1(...) MAP_GET_END2
#define MAP_GET_END(...) MAP_GET_END1
#define MAP_NEXT0(test, next, ...) next MAP_OUT
#define MAP_NEXT1(test, next) MAP_NEXT0(test, next, 0)
#define MAP_NEXT(test, next)  MAP_NEXT1(MAP_GET_END test, next)

#define MAP0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP1)(f, peek, __VA_ARGS__)
#define MAP1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP0)(f, peek, __VA_ARGS__)

#define MAP_LIST_NEXT1(test, next) MAP_NEXT0(test, MAP_COMMA next, 0)
#define MAP_LIST_NEXT(test, next)  MAP_LIST_NEXT1(MAP_GET_END test, next)

#define MAP_LIST0(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST1)(f, peek, __VA_ARGS__)
#define MAP_LIST1(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST0)(f, peek, __VA_ARGS__)

/**
 * Applies the function macro `f` to each of the remaining parameters.
 * From: https://github.com/swansontec/map-macro/blob/master/map.h
 */
#define MAP(f, ...) EVAL(MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

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
		__builtin_types_compatible_p(typeof(x), bool) ? "%d" : \
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
		__builtin_types_compatible_p(typeof(x), bool) ? "bool" : \
		__builtin_types_compatible_p(typeof(x), char) ? "char" : "void")

#define __1debug(x) \
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

#define debug(...) MAP(__1debug, __VA_ARGS__)

#endif /* macros_h */
