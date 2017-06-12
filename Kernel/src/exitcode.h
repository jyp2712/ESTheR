#ifndef EXITCODE_H_
#define EXITCODE_H_

#include "structures.h"

typedef enum {
	EXIT_SUCCESSFUL = 0,
	EXIT_RESOURCE_FAIL = -1,
	EXIT_FILE_NOT_FOUND = -2,
	EXIT_FILE_READ_FAIL = -3,
	EXIT_FILE_WRITE_FAIL = -4,
	EXIT_MEMORY_EXCEPTION = -5,
	EXIT_CONSOLE_KILLED = -6,
	EXIT_PROGRAM_KILLED = -7,
	EXIT_MALLOC_TOO_LARGE = -8,
	EXIT_PAGE_LIMIT_EXCEEDED = -9,
	EXIT_UNDEFINED_ERROR = -20
} exitcode_t;

void end_program(t_pcb *pcb);

t_pcb *duplicate_pcb(t_pcb *pcb);

void remove_pcb_from_lists(t_pcb *pcb);

#endif /* EXITCODE_H_ */
