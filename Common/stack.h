#ifndef stack_h
#define stack_h

#include <stddef.h>
#include <stdbool.h>

typedef struct stack stack_t;

stack_t *stack_create(void);

bool stack_empty(stack_t *stack);

int stack_length(stack_t *stack);

void *stack_top(stack_t *stack);

stack_t *stack_copy(stack_t *stack);

stack_t *stack_clone(stack_t *stack, size_t elem_size);

void stack_push(stack_t *stack, void *element);

void *stack_pop(stack_t *stack);

void stack_clear(stack_t *stack, void *destroyer);

void stack_destroy(stack_t *stack, void *destroyer);

#endif /* stack_h */
