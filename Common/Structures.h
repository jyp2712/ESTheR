#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <commons/collections/list.h>
#include <parser/sintax.h>
#include <parser/metadata_program.h>

typedef struct{
	int offset;
	int size;
}t_indexCode;

typedef struct{
	int program;
	int PC;
}t_programTag;

// Variable AnSISOP
typedef struct{
	char* id;
	int page;
	t_indexCode mempos;
}t_var; // o argumento, funcionan igual


typedef struct{
	int page;
	t_indexCode mempos;
}t_varRet;

// Elemento del Índice de Stack
typedef struct{
	int pos;
	t_list* args;
	t_list* vars;
	int retPos;
	t_varRet retVar;
}t_stack;

// PCB de un proceso
typedef struct {
	int idProcess;
	int PC;
	int status;
	int priority;
	int pagesCode;
	t_intructions* indexCode;
	t_programTag indexTag;
	t_list* indexStack;
	int exitCode;
} t_pcb;

#endif /* STRUCTURES_H_ */
