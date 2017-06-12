#include "exitcode.h"
#include "globals.h"
#include "protocol.h"
#include "Kernel.h"

void end_program(t_pcb *pcb) {

	bool get_pcb(t_pcb *element){
		return pcb->idProcess == element->idProcess;
	}

	pcb = duplicate_pcb(pcb);
	remove_pcb_from_lists(pcb);
	list_add(pcb_exit, pcb);

	unsigned char buffer[BUFFER_CAPACITY];
	int msgsize = serial_pack(buffer, "h", pcb->exitCode);

	header_t header = protocol_header(OP_KE_PROGRAM_END, msgsize);
	header.usrpid = pcb->idProcess;
	packet_t packet = protocol_packet(header, buffer);


	bool get_pid(int *pid) {
		return (int)pid == pcb->idProcess;
	}

	bool get_client(t_client *client) {
		return list_any_satisfy(client->pids, (void*) get_pid);
	}

	t_client *console = list_find(consolas_conectadas, (void*) get_client);

	if(console != NULL) {
		protocol_packet_send(packet, console->clientID);
	}

	packet.header.opcode = OP_ME_FINPRO;
	protocol_packet_send(packet, memfd);
	protocol_packet_receive(memfd);

	log_inform("Program #%d ended", pcb->idProcess);
}

t_pcb *duplicate_pcb(t_pcb *pcb) {
	t_pcb *dup = alloc(sizeof(t_pcb));
	memcpy(dup, pcb, sizeof(t_pcb));
	return dup;
}

void remove_pcb_from_lists(t_pcb *pcb) {
	bool condition(void *elem) {
		return elem == pcb;
	}

	t_list *lists[5] = {pcb_new, pcb_ready, pcb_exec, pcb_block, pcb_exit};
	for(int i = 0; i < 5; i++) {
		list_remove_and_destroy_by_condition(lists[i], condition, free);
	}
}
