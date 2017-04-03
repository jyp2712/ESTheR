#include <commons/collections/list.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 10			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define PACKAGESIZE 1024

/* ******************************************************************
 *                     	  TIPOS DE DATOS
 * *****************************************************************/

typedef struct{
	char* puerto_prog;
	char* puerto_cpu;
	char* ip_memoria;
	char* puerto_memoria;
	char* ip_fs;
	char* puerto_fs;
	int quantum;
	int quantum_sleep;
	char* algoritmo;
	int grado_multiprog;
	//Faltan semaforos
	int stack_size;
}t_kernel;


/* ******************************************************************
 *                     	   PROTOTIPOS DE
 *                           FUNCIONES
 * *****************************************************************/

void leerConfiguracionKernel(t_kernel*);


/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/

int main(){

	t_kernel* kernel = (t_kernel*) malloc(sizeof(t_kernel)); // Reservo memoria para la estructura del kernel

	leerConfiguracionKernel(kernel); //Leo configuracion metadata y la guardo en la estructura kernel

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	//Atributos para select
	fd_set master;		// conjunto maestro de descriptores de fichero
	fd_set read_fds;	// conjunto temporal de descriptores de fichero para select()
	int fdmax;			// número máximo de descriptores de fichero
	int newfd;			// descriptor de socket de nueva conexión aceptada
	int i;
	int nbytes;

	FD_ZERO(&master);	// borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(NULL, kernel->puerto_prog, &hints, &serverInfo);

	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	printf("Servidor esperando conexiones...\n");

	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	listen(listenningSocket, BACKLOG);

	// añadir listener al conjunto maestro
	FD_SET(listenningSocket, &master);
	// seguir la pista del descriptor de fichero mayor
	fdmax = listenningSocket; // por ahora es éste

	char package[PACKAGESIZE];

	//------------Comienzo del select------------
	for(;;) {
		read_fds = master; // cópialo
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}
		// explorar conexiones existentes en busca de datos que leer
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // ¡¡tenemos datos!!
				if (i == listenningSocket) {
					// gestionar nuevas conexiones
					addrlen = sizeof(addr);
					if ((newfd = accept(listenningSocket, (struct sockaddr*)&addr, &addrlen)) == -1){
						perror("accept");
					} else {
						FD_SET(newfd, &master); // añadir al conjunto maestro
						if (newfd > fdmax) {
							// actualizar el máximo
							fdmax = newfd;
						}
						printf("selectserver: new connection from %s on ""socket %d\n", inet_ntoa(addr.sin_addr),newfd);
					}
				} else {
					// gestionar datos de un cliente
					if ((nbytes = recv(i, (void*)package, PACKAGESIZE, 0)) <= 0) {
						// error o conexión cerrada por el cliente
						if (nbytes == 0) {
							// conexión cerrada
							printf("selectserver: socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						close(i);
						FD_CLR(i, &master); // eliminar del conjunto maestro
					} else {
						// tenemos datos de algún cliente
						if (nbytes != 0){
							printf("%s", package);
						}
					}
				}
			}
		}
	}

	close(listenningSocket);

	free(kernel);

	return 0;

}

/* ******************************************************************
 *                     		FUNCIONES
 * *****************************************************************/

void leerConfiguracionKernel(t_kernel* kernel){

		t_config* config = config_create("../metadata");

		kernel->puerto_prog = config_get_string_value(config, "PUERTO_PROG");
		kernel->puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
		kernel->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
		kernel->ip_fs = config_get_string_value(config, "IP_FS");
		kernel->puerto_fs = config_get_string_value(config, "PUERTO_FS");
		kernel->quantum = config_get_int_value(config, "QUANTUM");
		kernel->quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
		kernel->algoritmo = config_get_string_value(config, "ALGORITMO");
		kernel->grado_multiprog = config_get_int_value(config, "GRADO_MULTIPROG");
		kernel->stack_size = config_get_int_value(config, "STACK_SIZE");

		printf("---------------Mi configuracion---------------\n");
		printf("PUERTO_PROG: %s\n", kernel->puerto_prog);
		printf("PUNTO_CPU: %s\n", kernel->puerto_cpu);
		printf("IP_MEMORIA: %s\n", kernel->ip_memoria);
		printf("IP_FS: %s\n", kernel->ip_fs);
		printf("PUERTO_FS: %s\n", kernel->puerto_fs);
		printf("QUANTUM: %i\n", kernel->quantum);
		printf("QUANTUM_SLEEP: %i\n", kernel->quantum_sleep);
		printf("ALGORITMO: %s\n", kernel->algoritmo);
		printf("GRADO_MULTIPROG: %i\n", kernel->grado_multiprog);
		printf("STACK_SIZE: %i\n", kernel->stack_size);
		printf("----------------------------------------------\n");

}
