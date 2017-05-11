#include "stack.h"
#include "mlist.h"
#include <stdlib.h>

struct stack {
	mlist_t *content;
};

stack_t *stack_create() {
	stack_t *stack = malloc(sizeof(stack_t));
	stack->content = mlist_create();
	return stack;
}

bool stack_empty(stack_t *stack) {
	return mlist_empty(stack->content);
}

int stack_length(stack_t *stack) {
	return mlist_length(stack->content);
}

void *stack_top(stack_t *stack) {
	return mlist_last(stack->content);
}

stack_t *stack_copy(stack_t *stack) {
	stack_t *copy = stack_create();
	copy->content = mlist_copy(stack->content);
	return copy;
}

stack_t *stack_clone(stack_t *stack, size_t elem_size) {
	stack_t *clone = stack_create();
	clone->content = mlist_clone(stack->content, elem_size);
	return clone;
}

void stack_push(stack_t *stack, void *element) {
	mlist_append(stack->content, element);
}

void *stack_pop(stack_t *stack) {
	return mlist_pop(stack->content, stack_length(stack) - 1);
}

void stack_clear(stack_t *stack, void *destroyer) {
	mlist_clear(stack->content, destroyer);
}

void stack_destroy(stack_t *stack, void *destroyer) {
	mlist_destroy(stack->content, destroyer);
	free(stack);
}
