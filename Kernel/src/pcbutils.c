#include "pcbutils.h"
#include "Kernel.h"

#define NUMSTATUSES 5

string status_names[] = {"NEW", "READY", "EXEC", "BLOCK", "EXIT"};

static t_list *status_queue(int status) {
	switch(status) {
	case NEW: return pcb_new;
	case READY: return pcb_ready;
	case EXEC: return pcb_exec;
	case BLOCK: return pcb_block;
	case EXIT: return pcb_exit;
	}
	return NULL;
}

int pcb_status_from_name(string name) {
	char *str = string_duplicate((char*) name);
	string_to_upper(str);
	int i;
	for(i = 0; i < NUMSTATUSES && !streq(str, status_names[i]); i++);
	free(str);
	if(i == NUMSTATUSES) i = -1;
	return i;
}

void pcb_print_status_queue(int status) {
	int length = list_size(status_queue(status));
	printf("%s (%d)", pcb_status_name(status), length);
	if(length > 0) printf(": ");
	void iterator(void *elem) {
		t_pcb *pcb = (t_pcb*) elem;
		printf("%d, ", pcb->idProcess);
	}
	list_iterate(status_queue(status), iterator);
	if(length > 0) printf("\b\b\33[K");
	printf("\n");
}

string pcb_status_name(int status) {
	return status_names[status];
}

t_pcb *pcb_by_id(int pid) {
	bool condition(void *elem) {
		return ((t_pcb*)elem)->idProcess == pid;
	}

	for(int i = 0; i < NUMSTATUSES; i++) {
		t_pcb *pcb = list_find(status_queue(i), condition);
		if(pcb != NULL) return pcb;
	}

	return NULL;
}

t_pcb *pcb_duplicate(t_pcb *pcb) {
	t_pcb *dup = alloc(sizeof(t_pcb));
	memcpy(dup, pcb, sizeof(t_pcb));
	return dup;
}

bool pcb_equals(t_pcb *pcb1, t_pcb *pcb2) {
	return pcb1->idProcess == pcb2->idProcess;
}

int pcb_status(t_pcb *pcb) {
	bool condition(void *elem) {
		return pcb_equals(pcb, elem);
	}

	for(int i = 0; i < NUMSTATUSES; i++) {
		t_pcb *res = list_find(status_queue(i), condition);
		if(res != NULL) return i;
	}

	return -1;
}


void pcb_remove(t_pcb *pcb) {
	bool condition(void *elem) {
		return pcb_equals(pcb, elem);
	}

	for(int i = 0; i < NUMSTATUSES; i++) {
		list_remove_and_destroy_by_condition(status_queue(i), condition, free);
	}
}
