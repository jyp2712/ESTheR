#ifndef protocol_h
#define protocol_h

#include "socket.h"
#include "globals.h"

enum opcodes {
	OP_UNDEFINED,
	OP_HANDSHAKE,
	OP_RESPONSE,
	OP_ME_INIPRO,
	OP_ME_SOLBYTPAG,
	OP_ME_ALMBYTPAG,
	OP_ME_ASIPAGPRO,
	OP_ME_FINPRO,
	OP_ME_PROX_INST_REQUEST_OK,
	OP_ME_PROX_INST_REQUEST_FAIL,
	OP_KE_SEND_PID,
	OP_KE_SEND_PCB,
	OP_KE_SEND_STACK,
	OP_KE_SEND_STACK_SIZE,
	OP_KE_SEND_CODE,
	OP_KE_PROGRAM_END,
	OP_KE_SEND_QUANTUM,
	OP_NEW_PROGRAM,
	OP_CPU_TAMPAG_REQUEST,
	OP_CPU_PROX_INST_REQUEST,
	OP_CPU_PROGRAM_END,
	OP_CPU_SEMAPHORE,
	OP_PROGRAM_KILLED,
	OP_CPU_SHARED_VAR,
	OP_KE_SEND_SHAREDVALUE
};

typedef struct {
	unsigned char syspid;	// ID del proceso de sistema emisor del paquete
	unsigned char usrpid;	// ID del proceso de usuario emisor del paquete
	unsigned char opcode;	// Código de operación
	unsigned long msgsize;	// Tamaño del cuerpo del paquete
} header_t;

typedef struct {
	header_t header;
	unsigned char *payload;
} packet_t;

typedef struct {
	unsigned char usrpid;
	unsigned int pagreq;
} me_progpag_t;

header_t protocol_header_response(header_t header, unsigned long msgsize);

/**
 * Crea el encabezado de un paquete para una enviar una operación.
 * Se completan los campos syspid con el ID del proceso emisor y
 * opcode con el código de operación. El resto de los campos se
 * deja en cero.
 * @param opcode Código de operación
 * @param opcode (Opcional) Tamaño del cuerpo del paquete.
 * @return Encabezado del paquete.
 */
header_t protocol_header(unsigned char opcode, unsigned long msgsize);
#define protocol_header(...) DEFAULTS(protocol_header, 2, ##__VA_ARGS__, 0)

/**
 * Crea un paquete para enviar una operación.
 * @param header Encabezado del paquete.
 * @param payload (Opcional) Cuerpo del paquete.
 * @return Paquete.
 */
packet_t protocol_packet(header_t header, ...);

/**
 * Envía un packete a un determinado socket.
 * @param packet Paquete.
 * @param sockfd Descriptor del socket.
 */
void protocol_packet_send(packet_t packet, socket_t sockfd);

/**
 * Recibe un paquete de un determinado socket.
 * @param sockfd Descriptor del socket.
 * @return Paquete.
 */
packet_t protocol_packet_receive(socket_t sockfd);

/**
 * Hace un handshake con un determinado socket.
 * @param sockfd Descriptor del socket.
 */
void protocol_handshake_send(socket_t sockfd);

/**
 * Recibe un handshake de un determinado socket.
 * @param sockfd Descriptor del socket.
 * @return Tipo de proceso que manda el handshake.
 */
header_t protocol_handshake_receive(socket_t sockfd);

#endif /* protocol_h */
