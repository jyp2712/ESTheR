#include "syscall.h"

void gestion_syscall(packet_t cpu_syscall, t_client* cpu, socket_t mem_socket) {
    unsigned char buffer[BUFFER_CAPACITY];
    t_pcb* pcb = alloc(sizeof(t_pcb));
    serial_unpack_pcb(pcb, cpu_syscall.payload);
    bool getPcb (t_pcb *pcbExec){
    	return (pcb->idProcess == pcbExec->idProcess);
    }
    t_pcb* aux = list_remove_by_condition(pcb_exec, (void*)getPcb);
    free(aux);
	switch (cpu_syscall.header.opcode){
		case OP_CPU_PROGRAM_END:
			end_program(pcb, EXIT_SUCCESSFUL);
			restoreCPU(cpu);
			break;
		case OP_CPU_SEMAPHORE:
		{
			packet_t packet_sem = protocol_packet_receive(cpu->clientID);
			memcpy(buffer, packet_sem.payload+1, packet_sem.header.msgsize);

			int i=0;
			while(config->sem_ids[i]){
				if (strcmp(config->sem_ids[i], (char*)buffer) == 0) break;
				i++;
			}

			if (packet_sem.payload[0] == 'w'){
				sem_ansisop[i]--;
				if (sem_ansisop[i]>0){
					list_add(pcb_ready, pcb);
				}else{
					list_add(pcb_block, pcb);
					list_add(solicitudes_sem[i], pcb);
				}
			}else{
				sem_ansisop[i]++;
				list_add(pcb_block, pcb);
				list_add(solicitudes_sem[i], pcb);

				t_pcb* pcbReady = list_remove(solicitudes_sem[i], 0);
				list_add(pcb_ready, pcbReady);

				bool getPcb (t_pcb *pcbBlock){
					return (pcbReady->idProcess == pcbBlock->idProcess);
				}
				list_remove_by_condition(pcb_block, (void*)getPcb);
			}

			restoreCPU(cpu);
			break;
		}
		case OP_CPU_SHARED_VAR:
		{
			packet_t packet_shared_var = protocol_packet_receive(cpu->clientID);

			memcpy(buffer, packet_shared_var.payload+1, packet_shared_var.header.msgsize);
			char* variable = string_substring((char*)buffer, 0, packet_shared_var.header.msgsize-1);

			int i=0;
			while(config->shared_vars[i]){
				if (strcmp(config->shared_vars[i], variable) == 0) break;
				i++;
			}

			if (packet_shared_var.payload[0] == 'v'){
				header_t header_shared_var = protocol_header(OP_KE_SEND_SHAREDVALUE, serial_pack(buffer, "h", shared_values[i]));
				packet_t packet_shared_var = protocol_packet(header_shared_var, buffer);
				protocol_packet_send(packet_shared_var, cpu->clientID);
			}else{
				int value;
				packet_shared_var = protocol_packet_receive(cpu->clientID);
				serial_unpack(packet_shared_var.payload, "h", &value);
				shared_values[i] = value;
			}

			restoreCPU(cpu);
			break;
		}
	}
}
