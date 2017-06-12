#ifndef PCBUTILS_H_
#define PCBUTILS_H_

#include "structures.h"
#include "globals.h"

t_pcb *pcb_by_id(int pid);

int pcb_status_from_name(string name);

string pcb_status_name(int status);

void pcb_print_status_queue(int status);

t_pcb *pcb_duplicate(t_pcb *pcb);

bool pcb_equals(t_pcb *pcb1, t_pcb *pcb2);

int pcb_status(t_pcb *pcb);

void pcb_remove(t_pcb *pcb);

#endif /* PCBUTILS_H_ */
