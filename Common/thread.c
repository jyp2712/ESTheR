#include "thread.h"
#include <errno.h>
#include "utils.h"

static void ptcheck(int ret) {
	guard(ret == 0, strerror(ret));
}

thread_t _thread_create(void *(*routine)(void *), void *arg) {
	pthread_t thread;
	ptcheck(pthread_create(&thread, NULL, routine, arg));
	return thread;
}

static void signal_dummy_action(int signal) {
	signal++;
}

void _thread_signal_set(int signal, void (*routine)(int)) {
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_handler = routine != NULL ? routine : signal_dummy_action;

	ptcheck(sigaction(signal, &sa, NULL));
}

void thread_kill(thread_t thread) {
	pthread_kill(thread, SIGTERM);
	pthread_join(thread, NULL);
}
