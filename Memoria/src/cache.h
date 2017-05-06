#ifndef cache_h
#define cache_h

#include "utils.h"

void cache_init(unsigned n_entries, size_t s_entry, unsigned max_per_prog);

bool cache_write(int pid, int pageno, byte *page);

byte *cache_read(int pid, int pageno);

void cache_flush();

void cache_free();

#endif /* cache_h */
