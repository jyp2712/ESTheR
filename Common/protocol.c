#include "protocol.h"
#include "serial.h"
#include "log.h"

#define HEADER_SIZE 6

header_t protocol_header(unsigned char opcode, size_t msgsize) {
	header_t header;

	header.syspid = get_current_process();
	header.opcode = opcode;
	header.msgsize = msgsize;

	return header;
}

void protocol_header_send(header_t header, socket_t sockfd) {
	unsigned char buffer[HEADER_SIZE];
	size_t size = serial_pack(buffer, "CCL", header.syspid, header.opcode, header.msgsize);
	socket_send_bytes(buffer, size, sockfd);
}

int protocol_receive_header(socket_t sockfd, header_t *header) {
	unsigned char buffer[HEADER_SIZE];

	size_t r = socket_receive_bytes(buffer, HEADER_SIZE, sockfd);
	serial_unpack(buffer, "CCL", &header->syspid, &header->opcode, &header->msgsize);

	return r;
}

header_t protocol_header_receive(socket_t sockfd) {
	unsigned char buffer[HEADER_SIZE];
	header_t header;
	header.opcode = OP_UNDEFINED;

	socket_receive_bytes(buffer, HEADER_SIZE, sockfd);
	serial_unpack(buffer, "CCL", &header.syspid, &header.opcode, &header.msgsize);

	return header;
}

packet_t protocol_packet(header_t header, unsigned char *payload) {
	packet_t packet;

	packet.header = header;
	packet.payload = payload;

	return packet;
}

void protocol_packet_send(const packet_t packet, socket_t sockfd) {
	protocol_header_send(packet.header, sockfd);
	socket_send_bytes(packet.payload, packet.header.msgsize, sockfd);
	log_inform("Sent packet (%ld bytes)", packet.header.msgsize + HEADER_SIZE);
}

void protocol_packet_receive(packet_t *packet, socket_t sockfd) {
	packet->header = protocol_header_receive(sockfd);
	socket_receive_bytes(packet->payload, packet->header.msgsize, sockfd);
	log_inform("Received packet from %s (%ld bytes)",
			get_process_name(packet->header.syspid), packet->header.msgsize + HEADER_SIZE);
}

void protocol_send_handshake(socket_t sockfd) {
	protocol_header_send(protocol_header(OP_HANDSHAKE, 0), sockfd);
	log_inform("Sent handshake");
}
