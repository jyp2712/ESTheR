#ifndef protocol_h
#define protocol_h

#include "socket.h"
#include "globals.h"

#define OP_UNDEFINED 0
#define OP_HANDSHAKE 1

typedef struct {
	unsigned char syspid;
	unsigned char opcode;
	unsigned long msgsize;
} header_t;

typedef struct {
	header_t header;
	unsigned char *payload;
} packet_t;

/**
 * Crea el encabezado de un paquete para una enviar una operación.
 * @param opcode Código de operación
 * @param msgsize Tamaño del cuerpo del packete.
 * @return Encabezado del paquete.
 */
header_t protocol_header(unsigned char opcode, size_t msgsize);

/**
 * Crea un paquete para enviar una operación.
 * @param header Encabezado del paquete.
 * @param payload Cuerpo del paquete.
 * @return Paquete.
 */
packet_t protocol_packet(header_t header, unsigned char *payload);

/**
 * Envía un encabezado a un determinado socket.
 * @param header Encabezado.
 * @param sockfd Descriptor del socket.
 */
void protocol_header_send(header_t header, socket_t sockfd);

/**
 * Recibe un encabezado de un determinado socket.
 * @param sockfd Descriptor del socket.
 * @param header Cabecera del protocolo
 * @return Cantidad de bytes leidos, mismo comportamiento que recv.
 */
int protocol_receive_header(socket_t sockfd, header_t *header);

/**
 * Recibe un encabezado de un determinado socket.
 * @param sockfd Descriptor del socket.
 * @return Encabezado.
 */
header_t protocol_header_receive(socket_t sockfd);

/**
 * Envía un packete a un determinado socket.
 * @param packet Paquete.
 * @param sockfd Descriptor del socket.
 */
void protocol_packet_send(const packet_t packet, socket_t sockfd);

/**
 * Recibe un paquete de un determinado socket.
 * @param packet Paquete.
 * @param sockfd Descriptor del socket.
 */
void protocol_packet_receive(packet_t *packet, socket_t sockfd);

/**
 * Hace un handshake con un determinado socket.
 * @param sockfd Descriptor del socket.
 */
void protocol_send_handshake(socket_t sockfd);

#endif /* protocol_h */
