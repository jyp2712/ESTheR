#include "queue.h"
#include "mlist.h"
#include <stdlib.h>

struct queue {
	mlist_t *content;
};

queue_t *queue_create() {
	queue_t *queue = malloc(sizeof(queue_t));
	queue->content = mlist_create();
	return queue;
}

bool queue_empty(queue_t *queue) {
	return mlist_empty(queue->content);
}

int queue_length(queue_t *queue) {
	return mlist_length(queue->content);
}

void *queue_next(queue_t *queue) {
	return mlist_last(queue->content);
}

queue_t *queue_copy(queue_t *queue) {
	queue_t *copy = queue_create();
	copy->content = mlist_copy(queue->content);
	return copy;
}

queue_t *queue_clone(queue_t *queue, size_t elem_size) {
	queue_t *clone = queue_create();
	clone->content = mlist_clone(queue->content, elem_size);
	return clone;
}

void queue_push(queue_t *queue, void *element) {
	mlist_insert(queue->content, 0, element);
}

void *queue_pop(queue_t *queue) {
	return mlist_pop(queue->content, queue_length(queue) - 1);
}

void queue_clear(queue_t *queue, void *destroyer) {
	mlist_clear(queue->content, destroyer);
}

void queue_destroy(queue_t *queue, void *destroyer) {
	mlist_destroy(queue->content, destroyer);
	free(queue);
}
