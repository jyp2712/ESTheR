#include "protocol.h"
#include "serial.h"
#include "log.h"
#include <stdarg.h>

#define HEADER_SIZE 7

header_t protocol_header(unsigned char opcode, unsigned long msgsize) {
	header_t header;
	memset(&header, 0, sizeof header);
	header.syspid = get_current_process();
	header.opcode = opcode;
	header.msgsize = msgsize;
	return header;
}

static void send_header(header_t header, socket_t sockfd) {
	unsigned char buffer[HEADER_SIZE];
	size_t size = serial_pack(buffer, "CCCL", header.syspid, header.usrpid, header.opcode, header.msgsize);
	socket_send_bytes(buffer, size, sockfd);
}

static header_t receive_header(socket_t sockfd) {
	unsigned char buffer[HEADER_SIZE];
	header_t header;
	memset(&header, 0, sizeof header);

	if(socket_receive_bytes(buffer, HEADER_SIZE, sockfd) > 0) {
		serial_unpack(buffer, "CCCL", &header.syspid, &header.usrpid, &header.opcode, &header.msgsize);
	}

	return header;
}

packet_t protocol_packet(header_t header, ...) {
	packet_t packet;
	packet.header = header;
	if(header.msgsize == 0) {
		packet.payload = NULL;
	} else {
		va_list ap;
		va_start(ap, header);
		packet.payload = va_arg(ap, unsigned char *);
		va_end(ap);
	}
	return packet;
}

void protocol_response(socket_t sockfd, char *format, ...) {
	unsigned char buffer[1024];

	va_list ap;
	va_start(ap, format);

	size_t s = serial_pack_va(buffer, format, ap);
	socket_send_bytes(buffer, s, sockfd);
	log_inform("Sent response packet (%ld bytes)", s);
}


void protocol_packet_send(packet_t packet, socket_t sockfd) {
	send_header(packet.header, sockfd);
	socket_send_bytes(packet.payload, packet.header.msgsize, sockfd);
	log_inform("Sent packet (%ld bytes)", packet.header.msgsize + HEADER_SIZE);
}

packet_t protocol_packet_receive(socket_t sockfd) {
	packet_t packet;
	packet.header = receive_header(sockfd);

	if(packet.header.opcode != OP_UNDEFINED){
		packet.payload = malloc(packet.header.msgsize * sizeof(char));
		socket_receive_bytes(packet.payload, packet.header.msgsize, sockfd);
		log_inform("Received packet from %s (%ld bytes)",
				get_process_name(packet.header.syspid), packet.header.msgsize + HEADER_SIZE);
	}
	return packet;
}

void protocol_handshake_send(socket_t sockfd) {
	send_header(protocol_header(OP_HANDSHAKE), sockfd);
	log_inform("Sent handshake");
}

process_t protocol_handshake_receive(socket_t sockfd) {
	header_t header = receive_header(sockfd);
	if(header.opcode != OP_HANDSHAKE) {
		log_report("Received another operation while waiting for handshake");
	} else {
		log_inform("Received handshake from %s", get_process_name(header.syspid));
	}
	return header.syspid;
}
