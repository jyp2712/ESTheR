#include "page_table.h"

#define COLS 3
int page_rows;

t_page_table *get_page_table(int rows) {
	page_rows = rows;
	return alloc(sizeof(t_page_table) * rows * COLS);
}

int set_pages(t_page_table *page_table, int pid, int pages) {
	if(pid <= 0) {
		log_report("Invalid pid: %d. It must be a positive number.", pid);
		return 0;
	}
	if(pages <= 0) {
		log_report("Invalid requested pages: %d. It must be a positive number.", pages);
		return 0;
	}
	int i, r = 0;
	for(i = 0; i < page_rows; i++) {
		if(page_table[i * COLS] == 0) r++;
	}
	if(r < pages) {
		log_report("There is not %d free pages to be assigned.", pages);
		return 0;
	}

	for(i = 0; i < page_rows && pages > 0; i++) {
		if(page_table[i * COLS] == 0) {
			page_table[i * COLS] = pid;
			page_table[i * COLS + 1] = --pages;

		}
	}
	log_inform("Se asignaron %i paginas", i);
	return 1;
}

int clear_pages(t_page_table *page_table, int pid) {
	if(pid <= 0) {
		log_report("Invalid pid: %d. It must be a positive number.", pid);
		return 0;
	}
	int i = 0;

	for(i = 0; i < page_rows; i++) {
		if(page_table[i * COLS] == pid) {
			page_table[i * COLS] = 0;
			page_table[i * COLS + 1] = 0;
		}
	}
	return 1;
}

int get_frame(t_page_table *page_table, int pid, int page) {
	if(pid <= 0) {
		log_report("Invalid pid: %d. It must be a positive number.", pid);
		return 0;
	}
	if(page < 0) {
		log_report("Invalid requested page: %d. It must be zero or a positive number.", page);
		return 0;
	}
	if(page >= page_rows) {
		log_report("Requested page: %d out of boundaries.", page);
		return 0;
	}
	int i;
	for(i = 0; i < page_rows; i++) {
		if(page_table[i * COLS] == pid && page_table[i * COLS + 1] == page) return i;
	}
	log_report("Can't find page: %d for pid: %d.", page, pid);
	return 0;
}

int get_bytes(byte *main_memory, int frame, int offset, int size, byte *data) {
	if(offset < 0) {
		log_report("Invalid offset: %d. It must be zero or a positive number.", offset);
		return 0;
	}
	if(size <= 0) {
		log_report("Invalid size: %d. It must be a positive number.", size);
		return 0;
	}
	int i, j = 0;
	int base = frame * config->marco_size + offset;
	for(i = base; i < base + size; i++) {
		data[j++] = main_memory[i];
	}
	return 1;
}

int set_bytes(byte *main_memory, int frame, int offset, int size, byte *data) {
	if(offset < 0) {
		log_report("Invalid offset: %d. It must be zero or a positive number.", offset);
		return 0;
	}
	if(size <= 0) {
		log_report("Invalid size: %d. It must be a positive number.", size);
		return 0;
	}
	int i, j = 0;
	int base = frame * config->marco_size + offset;
	for(i = base; i < base + size; i++) {
		main_memory[i] = data[j++];
	}
	return 1;
}
