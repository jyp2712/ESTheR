#ifndef PAGE_TABLE_H_
#define PAGE_TABLE_H_

#include "utils.h"

typedef int t_page_table;

t_page_table *get_page_table(int rows);

int set_pages(t_page_table *page_table, int pid, int pages);

#endif /* PAGE_TABLE_H_ */
