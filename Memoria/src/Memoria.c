#include "Memoria.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"

#define SHMSZ 1

int shmid;
key_t key = 5678;

int main(int argc, char **argv) {
	char *shm;
    set_current_process(MEMORY);

	guard(argc == 2, "Falta indicar ruta de archivo de configuración");

	t_memoria* config = malloc(sizeof(t_memoria));
	leerConfiguracion(config, argv[1]);

	//inicializar memoria
	inicializar(config);

	pid_t id = fork();

	if(id > 0) {
		//crear servidor
		if((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)//, "No se pudo crear el id para la memoria compartida");
	    perror("servidor");
		if((shm = shmat(shmid, NULL, 0)) == (char *) -1)//, "No se pudo adjuntar el id a la memoria");
		perror("servidor");
		*shm = 'N';
		crearServidor(config, shm);
	}
	else if(id == 0) {
		//interprete de comandos
	    if((shmid = shmget(key, SHMSZ, 0666)) < 0)//, "No se pudo acceder el id para la memoria compartida");
	    perror("servidor");
	    if((shm = shmat(shmid, NULL, 0)) == (char *) -1)//, "No se pudo adjuntar el id a la memoria");
	    perror("servidor");
		interpreteDeComandos(config, shm);
	}
	else {
		exit(EXIT_FAILURE);
	}

	//liberar memoria

	free(config);

	return 0;
}

void leerConfiguracion(t_memoria* config, char* path){
	t_config* c = config_create(path);

	config->puerto = config_get_string_value(c, "PUERTO");
	config->marcos = config_get_int_value(c, "MARCOS");
	config->marco_size = config_get_int_value(c, "MARCO_SIZE");
	config->entradas_cache = config_get_int_value(c, "ENTRADAS_CACHE");
	config->cache_x_proc = config_get_int_value(c, "CACHE_X_PROC");
	config->reemplazo_cache = config_get_string_value(c, "REEMPLAZO_CACHE");
	config->retardo_memoria = config_get_int_value(c, "RETARDO_MEMORIA");

	printf("---------------Mi configuración---------------\n");
	printf("PUERTO: %s\n", config->puerto);
	printf("MARCOS: %i\n", config->marcos);
	printf("MARCO_SIZE: %i\n", config->marco_size);
	printf("ENTRADAS_CACHE: %i\n", config->entradas_cache);
	printf("CACHE_X_PROC: %i\n", config->cache_x_proc);
	printf("REEMPLAZO_CACHE: %s\n", config->reemplazo_cache);
	printf("RETARDO_MEMORIA: %i\n", config->retardo_memoria);
	printf("----------------------------------------------\n");
}

void inicializar(t_memoria* config){

}

void crearServidor(t_memoria* config, char *shm){
	struct timeval tv;
	int finServer = 0;
	fd_set master;
	fd_set read_fds;
	int fdmax;
	int i;

	tv.tv_sec = 4; //segundos
	tv.tv_usec = 500000; //microsegundos

	socket_t memoria_fd = socket_init(NULL, config->puerto);

	// add the listener to the master set
	FD_SET(memoria_fd, &master);
	// keep track of the biggest file descriptor
	fdmax = memoria_fd;

	while(!finServer) {
		read_fds = master; // copy it
		if (select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1) {
			//if (errno == EINTR) {
				// some signal just interrupted us, so restart
			//}
			guard(false, strerror(errno));
		}
		// run through the existing connections looking for data to read
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!!
				if (i == memoria_fd) {
					// handle new connections
					socket_t cli_sock = socket_accept_v2(memoria_fd);
					if(cli_sock != -1) {
						FD_SET(cli_sock, &master); // add to master set
						if (cli_sock > fdmax) {    // keep track of the max
							fdmax = cli_sock;
						}
					}
				}
				else {
					// handle data from a client
					header_t header;
					if(protocol_receive_header(i, &header) <= 0) {
						close(i);
						FD_CLR(i, &master); // remove from master set
					}
					else {
						// we got some data from a client
						if(validarHandshake(i, &header) == -1) {
							continue;
						}
						procesarMensaje(i);
					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
		if(*shm == 'F') finServer = 1;
	}

	socket_close(memoria_fd);
}

int validarHandshake(socket_t sockfd, header_t *header) {
	if((header->syspid == KERNEL && header->opcode == OP_HANDSHAKE)
			|| (header->syspid == CPU && header->opcode == OP_HANDSHAKE)) {
		//TODO responder por handshake ok
		return 0;
	}
	//TODO responder por handshake invalido
	log_inform("Invalid Handshake");
	return -1;
}

void procesarMensaje(socket_t sockfd) {

	printf("debería procesar el mensaje continuando su lectura de datos\n");
}

void interpreteDeComandos(t_memoria* config, char *shm){
	int finConsola = 0;
	char comando[80];

	printf("Escriba ? para ayuda de comandos disponibles\n");
	do {
		printf ("Comando: ");
		scanf ("%s", comando);

		if(strcmp(comando, "?") == 0) {
			printf("Comandos disponibles:\n");
			printf(" fin\n   Finaliza la terminal\n");
			printf(" dump\n   Dump de la memoria\n");
		}
		else if(strcmp(comando, "fin") == 0) {
			//terminar servidor
			finConsola = 1;
			*shm = 'F';
			printf("Proceso Memoria finalizado con éxito\n");
		}
		else if(strcmp(comando, "dump") == 0) {
			printf("Dump...\n");
		}
	} while(!finConsola);
	exit(EXIT_SUCCESS);
}
