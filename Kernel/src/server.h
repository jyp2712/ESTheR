#ifndef SERVER_H_
#define SERVER_H_

typedef struct{
    socket_t clientID;
    unsigned char process;
    t_list* pids;
}t_client;

void init_server(socket_t, socket_t);

t_client* buscar_proceso (socket_t);

void restoreCPU(t_client*);

#endif /* SERVER_H_ */

