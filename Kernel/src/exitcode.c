#include "exitcode.h"
#include "globals.h"
#include "protocol.h"
#include "Kernel.h"

void end_program(t_pcb *pcb, exitcode_t code) {
	pcb->exitCode = code;
	pcb = pcb_duplicate(pcb);
	pcb_remove(pcb);
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
