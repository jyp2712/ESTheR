#include "server.h"

#include <mlist.h>
#include "socket.h"
#include "thread.h"
#include "protocol.h"
#include "serial.h"
#include "log.h"
#include "Memoria.h"
#include "configuration.h"

t_memoria *config;
t_memory_data *memory;

struct {
	thread_t thread;
	mlist_t *clients;
	bool active;
} server;

typedef struct {
	thread_t thread;
	socket_t socket;
	process_t type;
} client_t;

void destroy_client(client_t *client) {
	socket_close(client->socket);
	thread_kill(client->thread);
	free(client);
}

void remove_client(client_t *client) {
	bool condition(client_t *element) {
		return element == client;
	}
	mlist_remove(server.clients, condition, destroy_client);
}

void cli_thread(client_t *client) {
	unsigned char buffer[BUFFER_CAPACITY];
	header_t header;
	packet_t packet;

	header = protocol_handshake_receive(client->socket);
	client->type = header.syspid;

	//Luego del handshake se indica al Kernel el tamaño del marco de página
	if(client->type == KERNEL) {
		header = protocol_header_response(header, serial_pack(buffer, "h", memory_get_frame_size()));
		packet = protocol_packet(header, buffer);
		protocol_packet_send(packet, client->socket);
	}

	while(true) {
		packet_t packet = protocol_packet_receive(client->socket);

		if(!server.active) return;

		log_inform("packet header syspid %d usrpid %d opcode %d msgsize %d", packet.header.syspid, packet.header.usrpid, packet.header.opcode, packet.header.msgsize);
		switch(packet.header.opcode) {
		case OP_ME_INIPRO:
		{
			int pages;
			serial_unpack(packet.payload, "h", &pages);

			int res = set_pages(memory->page_table, packet.header.usrpid, pages);

			header = protocol_header_response(header, serial_pack(buffer, "h", res));
			packet = protocol_packet(header, buffer);
			protocol_packet_send(packet, client->socket);
			break;
		}
		case OP_ME_SOLBYTPAG:
		{
			int page, offset, size;

			serial_unpack(packet.payload, "hhh", &page, &offset, &size);

			int frame = get_frame(memory->page_table, packet.header.usrpid, page);

			if(!get_bytes(memory->main, frame, offset, size, buffer)) {
				header = protocol_header_response(header, 0);
				packet = protocol_packet(header);
				protocol_packet_send(packet, client->socket);
				break;
			}

			header = protocol_header_response(header, size);
			packet = protocol_packet(header, buffer);
			protocol_packet_send(packet, client->socket);
			break;
		}
		case OP_ME_ALMBYTPAG:
		{
			int page, offset, size;

			serial_unpack(packet.payload, "hhh", &page, &offset, &size);
			packet_t packet = protocol_packet_receive(client->socket);

			int frame = get_frame(memory->page_table, packet.header.usrpid, page);

			if(!set_bytes(memory->main, frame, offset, size, packet.payload)) {
				header = protocol_header_response(header, 0);
				packet = protocol_packet(header);
				protocol_packet_send(packet, client->socket);
				break;
			}

			header = protocol_header_response(header, serial_pack(buffer, "h", 1));
			packet = protocol_packet(header, buffer);
			protocol_packet_send(packet, client->socket);
			break;
		}
		case OP_ME_ASIPAGPRO:
			break;
		case OP_ME_FINPRO:
			break;
		case OP_UNDEFINED:
		default:
			remove_client(client);
			return;
		}
	}
}

//void *process_signal(int sig, siginfo_t *info, void *ucontext)
//{
//	printf("atiende señal\n");
//	server.active = false;
//	return NULL;
//}

void srv_thread(int port) {
	//thread_signal_set(SIGTERM, process_signal);

	char *port_str = string_itoa(port);
	socket_t srv_sock = socket_init(NULL, port_str);
	free(port_str);

	server.clients = mlist_create();

	socket_t cli_sock;
	while(server.active) {
		cli_sock = socket_accept(srv_sock);
		if(cli_sock != -1) {
			client_t *client = alloc(sizeof(client_t));
			client->socket = cli_sock;
			mlist_append(server.clients, client);
			client->thread = thread_create(cli_thread, client);
		}
	}

	mlist_destroy(server.clients, destroy_client);
	socket_close(srv_sock);
}

void server_start(t_memoria *c, t_memory_data *m) {
	config = c;
	memory = m;
	server.active = true;
	server.thread = thread_create(srv_thread, config->puerto);
}

void server_end() {
	server.active = false;
	thread_kill(server.thread);
}
