#ifndef PAGE_TABLE_H_
#define PAGE_TABLE_H_

#include "utils.h"
#include "globals.h"

typedef int t_page_table;

t_page_table *get_page_table(int rows, int frame_size);

int set_pages(t_page_table *page_table, int pid, int pages);

int clear_pages(t_page_table *page_table, int pid);

int get_frame(t_page_table *page_table, int pid, int page);

int get_bytes(byte *main_memory, int frame, int offset, int size, byte *data);

int set_bytes(byte *main_memory, int frame, int offset, int size, byte *data);

#endif /* PAGE_TABLE_H_ */
