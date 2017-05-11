#ifndef queue_h
#define queue_h

#include <stddef.h>
#include <stdbool.h>

typedef struct queue queue_t;

queue_t *queue_create(void);

bool queue_empty(queue_t *queue);

int queue_length(queue_t *queue);

void *queue_next(queue_t *queue);

queue_t *queue_copy(queue_t *queue);

queue_t *queue_clone(queue_t *queue, size_t elem_size);

void queue_push(queue_t *queue, void *element);

void *queue_pop(queue_t *queue);

void queue_clear(queue_t *queue, void *destroyer);

void queue_destroy(queue_t *queue, void *destroyer);

#endif /* queue_h */
