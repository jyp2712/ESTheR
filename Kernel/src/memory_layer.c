#include "memory_layer.h"
#include "mlist.h"

typedef struct {
	int pid;
	int pageno;
	byte *content;
} page_t;

mlist_t pages;

void memory_layer_init() {
	pages = mlist_create();
}

void memory_layer_destroy() {
	void destroyer(page_t *page) {
		free(page->content);
		free(page);
	}
	mlist_destroy(pages, destroyer);
}

void segment_alloc(byte *buffer, int size) {

}
