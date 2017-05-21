#include "page_table.h"

#define COLS 3
int page_rows;
int frame_size;

t_page_table *get_page_table(int rows, int size) {
	page_rows = rows;
	frame_size = size;
	return alloc(sizeof(t_page_table) * rows * COLS);
}

int set_pages(t_page_table *page_table, int pid, int pages) {
	int i, r = 0;
	for(i = 0; i < page_rows; i++) {
		if(page_table[i * COLS] == 0) r++;
	}
	if(r < pages) return -1;

	for(i = 0; i < page_rows && pages > 0; i++) {
		if(page_table[i * COLS] == 0) {
			page_table[i * COLS] = pid;
			pages--;
		}
	}
	return 0;
}

int get_frame(t_page_table *page_table, int pid, int page) {
	int i;
	for(i = 0; i < page_rows; i++) {
		if(page_table[i * COLS] == pid && page_table[i * COLS + 1] == page) return i;
	}
	return -1;
}

int get_bytes(byte *main_memory, int frame, int offset, int size, byte *data) {
	int i, j = 0;
	int base = frame * frame_size + offset;
	for(i = base; i < base + size; i++) {
		data[j++] = main_memory[i];
	}
	return 0;
}

int set_bytes(byte *main_memory, int frame, int offset, int size, byte *data) {
	int i, j = 0;
	int base = frame * frame_size + offset;
	for(i = base; i < base + size; i++) {
		main_memory[i] = data[j++];
	}
	return 0;
}
