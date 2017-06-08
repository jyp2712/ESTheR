#ifndef MEMORY_LAYER_H_
#define MEMORY_LAYER_H_

#include "utils.h"
#include <stdint.h>

typedef struct {
	uint32_t size;
	bool is_free;
} heap_metadata_t;

#endif /* MEMORY_LAYER_H_ */
