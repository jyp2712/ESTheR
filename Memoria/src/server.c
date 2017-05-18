#include "server.h"

#include <mlist.h>
#include "socket.h"
#include "thread.h"
#include "protocol.h"
#include "serial.h"
#include "log.h"
#include "Memoria.h"

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

	client->type = protocol_handshake_receive(client->socket);

	//Luego del handshake se indica al Kernel el tamaño del marco de página
	if(client->type == KERNEL) {
		size_t s = serial_pack(buffer, "h", memory_get_frame_size());
		socket_send_bytes(buffer, s, client->socket);
		log_inform("Sent init data memory (%d bytes)", s);
	}

	while(true) {
		packet_t packet = protocol_packet_receive(client->socket);

		if(!server.active) {
			return;
		}

		switch(packet.header.opcode) {
		case OP_ME_INIPRO:
		{
			t_memreq req;
			serial_unpack_memreq(&req, packet.payload);

			break;
		}
		case OP_ME_SOLBYTPAG:
			break;
		case OP_ME_ALMBYTPAG:
			break;
		case OP_ME_ASIPAGPRO:
			break;
		case OP_ME_FINPRO:
			break;
		case OP_CPU_TAMPAG_REQUEST:
		{
			header_t header = protocol_header(OP_CPU_TAMPAG_REQUEST);
			header.msgsize = serial_pack(buffer, "h", memory_get_frame_size());

			packet_t packet = protocol_packet(header, buffer);
			protocol_packet_send(packet, client->socket);
			break;
		}
		case OP_CPU_PROX_INST_REQUEST:
		{
			t_solicitudLectura* direccionInstruccion;
			// Recibo pagina, offset de inicio y tamaño de lo que tengo que leer y enviar
			direccionInstruccion = alloc(sizeof(t_solicitudLectura));
			serial_unpack(buffer, "hhhh", &direccionInstruccion->idProcess ,&direccionInstruccion->page, &direccionInstruccion->offset, &direccionInstruccion->size);

			// Validar si se puede acceder a esa direccion y responder con Ok o Fail (mirar como esta en CPU)
			header_t header = protocol_header(OP_ME_PROX_INST_REQUEST_OK);
			header.msgsize = serial_pack(buffer, "s", "variables a, b");
			packet_t packet2 = protocol_packet (header, buffer);
			protocol_packet_send(packet2, client->socket);
			break;
		}
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

void server_start(int port) {
	server.active = true;
	server.thread = thread_create(srv_thread, port);
}

void server_end() {
	server.active = false;
	thread_kill(server.thread);
}
