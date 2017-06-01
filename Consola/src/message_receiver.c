#include "message_receiver.h"
#include "protocol.h"
#include "mlist.h"
#include "Consola.h"

program_t *get_program(int pid) {
	bool condition(program_t *element) {
		return element->pid == pid;
	}
	return mlist_find(console.programs, condition);
}

void message_receiver() {
	while(console.active) {
		packet_t packet = protocol_packet_receive(console.kernel);
		program_t *program;
		switch(packet.header.opcode) {
		case OP_UNDEFINED:
			terminate();
			return;
		case OP_KE_SEND_PID:
			program = mlist_last(console.programs);
			program->pid = packet.header.usrpid;
			print("Programa #%d iniciado.", program->pid);
			break;
		default:
			program = get_program(packet.header.usrpid);
			console.message = (char*) packet.payload;
			thread_sem_signal(&program->sem);
		}
	}
}
