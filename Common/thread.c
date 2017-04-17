#include "thread.h"
#include <errno.h>
#include "utils.h"

static void ptcheck(int ret) {
	guard(ret == 0, strerror(ret));
}

thread_t thread_create(void *(*routine)(void *), void *arg) {
	pthread_t thread;
	ptcheck(pthread_create(&thread, NULL, routine, arg));
	return thread;
}
