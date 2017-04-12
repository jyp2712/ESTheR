#include "CPU.h"
#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "log.h"

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CPU);

	t_cpu* cpu = malloc(sizeof(t_cpu));
	leerConfiguracionCPU(cpu, argv[1]);

	puts("Conectándose al Kernel...");
	int kernel_fd = socket_connect(cpu->ip_kernel, cpu->puerto_kernel);
	protocol_send_handshake(kernel_fd);
	puts("Conectado.");

	// Harcodeo en un array el codigo de facil.anSISOP para recorrer con el analizador de linea.
	char* instruccion[6];
	instruccion[0] = "#!/usr/bin/ansisop";
	instruccion[1] = "begin";
	instruccion[2] = "variables a, b";
	instruccion[3] = "a = 3";
	instruccion[4] = "b = 5";
	instruccion[5] = "a = b + 12";
	instruccion[6] = "end";

	int i;
	for (i = 0; i < 7; ++i) {
		log_inform("La instruccion a parsear es: %s", instruccion[i]);
		analizadorLinea(instruccion[i], &funcionesAnSISOP, &funcionesKernel);
	}

	//------------Envio de mensajes al servidor------------
	char message[SOCKET_BUFFER_CAPACITY];

	while(socket_receive_string(message, kernel_fd) > 0) {
		printf("Recibido mensaje: \"%s\"\n", message);
	}

	free(cpu);
	socket_close(kernel_fd);
	return 0;
}

void leerConfiguracionCPU(t_cpu* cpu, char* path) {

	t_config* config = config_create(path);

	cpu->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	cpu->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	printf("---------------Mi configuración---------------\n");
	printf("IP KERNEL: %s\n", cpu->ip_kernel);
	printf("PUERTO KERNEL: %s\n", cpu->puerto_kernel);
	printf("----------------------------------------------\n");
}

// Primitivas AnSISOP

t_puntero definirVariable(t_nombre_variable identificador_variable){

	t_puntero posicion;

	log_inform("Definir variable");

	return posicion;
}

// @return	Donde se encuentre la variable buscada
t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){

	t_puntero posicion;

	malloc(identificador_variable);
	log_inform("Obtengo posicion de %s", &identificador_variable);

	return posicion;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){

	t_valor_variable valorVariable;

	log_inform("Dereferenciar");

	return valorVariable;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor){

	log_inform("Asignar valor");
}


void llamarSinRetorno(t_nombre_etiqueta etiqueta){

	log_inform("Llamar sin retorno");

}

void finalizar(void){

	log_inform("Finalizar");

}

