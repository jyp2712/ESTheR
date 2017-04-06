#ifndef COMMONSOCKETS_H_
#define COMMONSOCKETS_H_

#define BACKLOG 5

void create_socketClient(int* serverSocket, char* ip, char* port);
void create_serverSocket(int* listenningSocket, char* port);
void accept_connection(int listenningSocket, int* clientSocket);

#endif /* COMMONSOCKETS_H_ */
