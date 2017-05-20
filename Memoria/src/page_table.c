#include "page_table.h"

#define COLS 3
int page_rows;

t_page_table *get_page_table(int rows) {
	page_rows = rows;
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
