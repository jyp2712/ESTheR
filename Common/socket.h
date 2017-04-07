//void create_socketClient(int* serverSocket, char* ip, char* port);
//void create_serverSocket(int* listenningSocket, char* port);
//void accept_connection(int listenningSocket, int* clientSocket);

#ifndef socket_h
#define socket_h

#include "utils.h"

#define SOCKET_BUFFER_CAPACITY 1024
#define SOCKET_BACKLOG 5
#define SOCKET_PRINT_OPERATIONS true
#define SOCKET_MAX_POLLED 10

/**
 * Crea un socket de servidor para conectarse con un cliente a través de un
 * puerto determinado.
 * @param port Puerto de escucha.
 * @return Descriptor del socket del cliente.
 */
int socket_listen(const char *port);

/**
 * Crea un socket de cliente para conectarse con un servidor en una dirección
 * IP y un puerto determinados.
 * @param ip Dirección IP del servidor.
 * @param port Puerto del servidor.
 * @return Descriptor del socket del servidor.
 */
int socket_connect(const char *ip, const char *port);

/**
 * Envía un mensaje por una conexión abierta en un determinado socket.
 * @param message Mensaje a enviar.
 * @param sockfd Descriptor del socket.
 * @return Número de bytes enviados.
 */
ssize_t socket_send(const char *message, int sockfd);

/**
 * Recibe un mensaje por una conexión abierta en un determinado socket.
 * @param message Mensaje a recibir.
 * @param sockfd Descriptor del socket.
 * @return Número de bytes recibidos.
 */
ssize_t socket_receive(char *message, int sockfd);

/**
 * Crea un servidor multicliente usando select(), acepta conexiones a un cierto
 * puerto, recibe datos y los replica a todos los clientes conectados y servidores.
 * @param port Puerto de escucha.
 */
void socket_select(const char *port, int portServer1, int portServer2);

/**
 * Crea un servidor multicliente usando poll(), acepta conexiones a un cierto
 * puerto, recibe datos y los replica a todos los clientes conectados.
 * @param port Puerto de escucha.
 */
void socket_poll(const char *port);

/**
 * Cierra un socket abierto con socket_listen() o socket_connect().
 * @param sockfd Descriptor del socket a cerrar.
 */
void socket_close(int sockfd);

#endif /* socket_h */
