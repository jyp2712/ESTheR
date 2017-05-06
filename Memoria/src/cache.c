#include "cache.h"
#include "commons/collections/queue.h"
#include "log.h"

typedef struct {
	int pid;
	int pageno;
	byte *page;
} cache_entry_t;

struct {
	bool enabled;    // Indica si se usa la memoria caché
	int mepp;        // Máximo de entradas por programa
	size_t s_entry;  // Tamaño de entrada (igual al tamaño de marco y de página)
	byte *content;   // Bloque de memoria caché
	t_list *entries; // Lista de entradas (cache_entry_t)
} cache;

void cache_create(unsigned n_entries, size_t s_entry, unsigned max_per_prog) {
	if(n_entries == 0) {
		cache.enabled = false;
		return;
	}
	cache.enabled = true;
	cache.mepp = max_per_prog;
	cache.s_entry = s_entry;
	cache.content = alloc(n_entries * s_entry * sizeof(byte));
	cache.entries = list_create();
	for(unsigned i = 0; i < n_entries; i++) {
		cache_entry_t *entry = alloc(sizeof(cache_entry_t));
		entry->pid = -1;
		entry->pageno = -1;
		entry->page = cache.content + i * s_entry * sizeof(byte);
		list_add(cache.entries, entry);
	}
	log_inform("Created cache with %d entries of %d bytes", n_entries, s_entry);
}

bool cache_write(int pid, int pageno, byte *page) {
	if(!cache.enabled) {
		return false;
	}
	bool same_pid(cache_entry_t *entry) {
		return entry->pid == pid;
	}
	bool same_pageno(cache_entry_t *entry) {
		return entry->pageno == pageno;
	}

	t_list *p_entries = list_filter(cache.entries, (bool(*)(void*)) same_pid);
	cache_entry_t *entry = list_find(p_entries, (bool(*)(void*)) same_pageno);
	int epp = list_size(p_entries);
	list_destroy(p_entries);

	if(entry == NULL) {
		if(epp == cache.mepp) return false;
		bool entry_is_empty(cache_entry_t *entry) {
			return entry->pid == -1;
		}
		entry = list_find(cache.entries, (bool(*)(void*)) entry_is_empty);
		if(entry == NULL) {
			// Reemplazar la entrada menos recientemente usada (LRU)
			entry = list_get(cache.entries, 0);
		}
	}

	entry->pid = pid;
	entry->pageno = pageno;
	memcpy(entry->page, page, cache.s_entry);
	log_inform("Written cache record from process #%d, page #%d", pid, pageno);

	return true;
}

byte *cache_read(int pid, int pageno) {
	if(!cache.enabled) {
		return NULL;
	}
	int index = -1, i = 0;
	void iterator(cache_entry_t *entry) {
		if(entry->pid == pid && entry->pageno == pageno) {
			index = i;
		}
		i++;
	}
	list_iterate(cache.entries, (void(*)(void*)) iterator);;
	if(index == -1) return NULL;

	// Los usados más recientemente van al final de la lista
	cache_entry_t *entry = list_remove(cache.entries, index);
	list_add(cache.entries, entry);
	log_inform("Read cache record from process #%d, page #%d", pid, pageno);

	return entry->page;
}

void cache_flush() {
	if(!cache.enabled) return;
	void iterator(cache_entry_t *entry) {
		entry->pid = -1;
		entry->pageno = -1;
	}
	list_iterate(cache.entries, (void(*)(void*)) iterator);
	log_inform("Cache flushed");
}

void cache_destroy() {
	if(!cache.enabled) return;
	void destroyer(void *entry) {
		free(entry);
	}
	list_destroy_and_destroy_elements(cache.entries, destroyer);
	free(cache.content);
	cache.enabled = false;
	log_inform("Cache destroyed");
}
