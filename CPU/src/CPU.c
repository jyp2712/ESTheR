#include "CPU.h"
#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CPU);

	t_cpu* cpu = malloc(sizeof(t_cpu));
	leerConfiguracionCPU(cpu, argv[1]);

	puts("Conectándose al Kernel...");
	int kernel_fd = socket_connect(cpu->ip_kernel, cpu->puerto_kernel);
	protocol_send_handshake(kernel_fd);
	puts("Conectado.");

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

	return posicion;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){
	t_puntero posicion;

	return posicion;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){
	t_valor_variable valorVariable;

	return valorVariable;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor){

}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	t_valor_variable valorVariable;

	return valorVariable;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	t_valor_variable valorVariable;

	return valorVariable;
}

void irAlLabel(t_nombre_etiqueta t_nombre_etiqueta){

}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){

}

void llamarSinRetorno(t_nombre_etiqueta etiqueta){

}

void retornar(t_valor_variable retorno){

}

void finalizar(void){

}

