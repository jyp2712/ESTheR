#ifndef socket_h
#define socket_h

#include "utils.h"

#define SOCKET_BUFFER_CAPACITY 1024
#define SOCKET_BACKLOG 5
#define SOCKET_PRINT_OPERATIONS true
#define SOCKET_MAX_POLLED 10

typedef int socket_t;

typedef struct {
    fd_set set;
    socket_t max;
} fdset_t;

/**
 * Crea un socket de servidor para conectarse con un cliente a través de un
 * puerto determinado.
 * @param port Puerto de escucha.
 * @return Descriptor del socket del cliente.
 */
socket_t socket_listen(const char *port);

/**
 * Crea un socket de cliente para conectarse con un servidor en una dirección
 * IP y un puerto determinados.
 * @param ip Dirección IP del servidor.
 * @param port Puerto del servidor.
 * @return Descriptor del socket del servidor.
 */
socket_t socket_connect(const char *ip, const char *port);

/**
 * Envía un mensaje por una conexión abierta en un determinado socket.
 * @param message Mensaje a enviar.
 * @param sockfd Descriptor del socket.
 * @return Número de bytes enviados.
 */
size_t socket_send(const char *message, socket_t sockfd);

/**
 * Recibe un mensaje por una conexión abierta en un determinado socket.
 * @param message Mensaje a recibir.
 * @param sockfd Descriptor del socket.
 * @return Número de bytes recibidos.
 */
size_t socket_receive(char *message, socket_t sockfd);

/**
 * Crea un conjunto de sockets para ser usado por socket_select().
 * @return Conjunto de sockets.
 */
fdset_t socket_set_create(void);

/**
 * Agrega un socket a un conjunto de sockets.
 * @param fd socket a agregar.
 * @param fds conjunto de sockets.
 */
void socket_set_add(socket_t fd, fdset_t *fds);

/**
 * Crea un servidor multicliente usando select(), acepta conexiones a un cierto
 * puerto, recibe datos y los replica a todos los clientes conectados.
 * @param port Puerto de escucha.
 * @param sockfds Otros sockets a quienes replicar los datos recibidos.
 */
void socket_select(const char *port, const fdset_t *sockfds);

/**
 * Cierra un socket abierto con socket_listen() o socket_connect().
 * @param sockfd Descriptor del socket a cerrar.
 */
void socket_close(socket_t sockfd);

#endif /* socket_h */
