#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <commons/collections/list.h>
#include <parser/sintax.h>

typedef struct{
	int page;
	int offset;
	int size;
}t_indexCode;

typedef struct{
	int program;
	int PC;
}t_programTag;

typedef struct{
	char* id;
	t_indexCode mempos;
}t_var;

typedef struct{
	int pos;
	t_var args;
	t_var vars;
	int retPos;
	t_indexCode retVar;
}t_stack;

typedef struct {
	int idProcess;
	int PC;
	int status;
	int priority;
	int pagesCode;
	t_indexCode indexCode [PROGRAM_LINES];
	t_programTag indexTag;
	t_list* indexStack;
	int exitCode;
} t_pcb;

#endif /* STRUCTURES_H_ */
