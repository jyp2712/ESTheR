#ifndef MEMLAYER_H_
#define MEMLAYER_H_

#include "utils.h"
#include <stdint.h>

typedef struct {
	uint32_t size;
	bool is_free;
} heap_metadata_t;

#endif /* MEMLAYER_H_ */
