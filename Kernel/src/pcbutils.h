#ifndef PCBUTILS_H_
#define PCBUTILS_H_

#include "structures.h"

t_pcb *pbc_by_id(int pid);

t_pcb *pbc_duplicate(t_pcb *pcb);

bool pcb_equals(t_pcb *pcb1, t_pcb *pcb2);

int pcb_status(t_pcb *pcb);

void pcb_remove(t_pcb *pcb);

#endif /* PCBUTILS_H_ */
