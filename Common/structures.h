#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <commons/collections/list.h>
#include <parser/sintax.h>
#include <parser/metadata_program.h>

#define t_instruction t_intructions

/**
 * Estructura para identificar una posición de memoria por medio
 * de tres variables: su número de página (page), su desplazamiento
 * dentro de la página (offset) y el tamaño que ocupa (size).
 */
typedef struct{
	int page;
	int offset;
	int size;
}t_indexCode;

typedef struct{
	char program;
	int PC;
}t_programTag;

// Variable AnSISOP
typedef struct{
	char id;
	t_indexCode mempos;
}t_var; // o argumento, funcionan igual

// Elemento del Índice de Stack (en definitiva una función)
typedef struct{
	int pos;
	t_list* args;
	t_list* vars;
	int retPos;					// Numero de instruccion a la que debe retornar.
	t_indexCode retVar;			// Posición de memoria donde almacenar la variable de retorno.
}t_stack;

// PCB de un proceso
typedef struct {
	int idProcess;				// ID del programa
	int PC;						// Contador del programa
	int status;					// Estado del programa
	int priority;				// Prioridad??? (yo la sacaría)
	int pagesCode;				// Cantidad de páginas de memoria que ocupa el código
	t_instruction* indexCode;	// Conjunto de instrucciones del programa
	t_programTag indexTag;		// (Esto todavía no entiendo bien qué es) <-- alguien que lo entienda que reemplace este comentario
	t_list* indexStack;			// Lista con los elementos de la pila de usuario del programa.
	int offsetStack;			// Offset actual del stack
	int pageStack;				// Pagina actual del stack
	int exitCode;				// Código de retorno del programa.
} t_pcb;

#endif /* STRUCTURES_H_ */
