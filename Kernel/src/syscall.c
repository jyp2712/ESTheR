#include "Kernel.h"
#include "syscall.h"

void gestion_syscall(packet_t cpu_syscall, t_client* cpu, socket_t mem_socket){
    unsigned char buffer[BUFFER_CAPACITY];

	switch (cpu_syscall.header.opcode){
		case OP_CPU_PROGRAM_END:{
									t_pcb* pcb = alloc(sizeof(t_pcb));
									serial_unpack_pcb(pcb, cpu_syscall.payload);
									bool getPcb (t_pcb *pcbExec){
										return (pcb->idProcess == pcbExec->idProcess);
									}
									t_pcb* aux = list_remove_by_condition(pcb_exec, (void*)getPcb);
									free(aux);
									list_add(pcb_exit, pcb);

									bool getPid (int *pid){
										return (pid == (int*)pcb->idProcess);
									}

									bool getClient (t_client *client){
										return list_any_satisfy(client->pids, (void*)getPid);
									}

									t_client* console = list_find(consolas_conectadas, (void*)getClient);

									header_t header_program_end = protocol_header(OP_KE_PROGRAM_END, serial_pack(buffer, "h", pcb->exitCode));
								    header_program_end.usrpid = pcb->idProcess;
									packet_t packet_program_end = protocol_packet(header_program_end, buffer);
								    protocol_packet_send(packet_program_end, console->clientID);
								    packet_program_end.header.opcode = OP_ME_FINPRO;
								    protocol_packet_send(packet_program_end, mem_socket);

								    packet_program_end = protocol_packet_receive(mem_socket);
									restoreCPU(cpu);
									break;
		}
		case OP_CPU_SEMAPHORE:{		t_pcb* pcb = alloc(sizeof(t_pcb));
									serial_unpack_pcb(pcb, cpu_syscall.payload);
									bool getPcb (t_pcb *pcbExec){
									return (pcb->idProcess == pcbExec->idProcess);
									}
									t_pcb* aux = list_remove_by_condition(pcb_exec, (void*)getPcb);
									free(aux);
									packet_t packet_sem = protocol_packet_receive(cpu->clientID);
									int op_sem;
									char sem[20];
									serial_unpack(packet_sem.payload, "h20s", &op_sem, &sem);
									int i=0;int semValue;
									while(config->sem_ids[i]){
										if (strcmp(config->sem_ids[i], sem) == 0) break;
									}
									if (op_sem == 0){
										sem_wait(&config->sem_ansisop[i]);
										sem_getvalue(&config->sem_ansisop[i], &semValue);
										if (semValue>0){
											list_add(pcb_ready, pcb);
										}else{
											list_add(pcb_block, pcb);
											list_add(config->solicitudes_sem[i], pcb);
										}
									}else{
										sem_post(&config->sem_ansisop[i]);
										list_add(pcb_block, pcb);
										list_add(config->solicitudes_sem[i], pcb);
										t_pcb* pcbReady = list_remove(config->solicitudes_sem[i], 0);
										list_add(pcb_ready, pcbReady);
;
										bool getPcb (t_pcb *pcbBlock){
											return (pcbReady->idProcess == pcbBlock->idProcess);
										}
										t_pcb* aux = list_remove_by_condition(pcb_block, (void*)getPcb);
										free(aux);
									}

									restoreCPU(cpu);
									break;
		}
		case OP_CPU_SHARED_VAR:{	t_pcb* pcb = alloc(sizeof(t_pcb));
									serial_unpack_pcb(pcb, cpu_syscall.payload);
									bool getPcb (t_pcb *pcbExec){
										return (pcb->idProcess == pcbExec->idProcess);
									}
									t_pcb* aux = list_remove_by_condition(pcb_exec, (void*)getPcb);
									free(aux);
									packet_t packet_shared_var = protocol_packet_receive(cpu->clientID);
									char shared_var[20];
									serial_unpack(packet_shared_var.payload, "20s", &shared_var);
									int i=0;
									while(config->shared_vars[i]){
										if (strcmp(config->sem_ids[i], shared_var) == 0) break;
									}
									if (true/*packet_shared_var.header.opcode == get shared value*/){
										header_t header_shared_var = protocol_header(OP_KE_SEND_SHAREDVALUE, serial_pack(buffer, "h", config->shared_values[i]));
										packet_t packet_shared_var = protocol_packet(header_shared_var, buffer);
										protocol_packet_send(packet_shared_var, cpu->clientID);
									}else{
										int value;
										serial_unpack(packet_shared_var.payload+20, "h", &value);
										config->shared_values[i] = value;
									}

									restoreCPU(cpu);
									break;
		}
	}
}
