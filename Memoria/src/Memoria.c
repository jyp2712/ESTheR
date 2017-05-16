#include "Memoria.h"
#include <commons/config.h>
#include <commons/collections/list.h>
#include "log.h"
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "thread.h"
#include "Configuracion.h"
#include "operations.h"
#include "serial.h"
#include "structures.h"
#include "cache.h"
#include "server.h"
#include "console.h"

t_memoria *config;

// Estado global de la Memoria
struct {
	byte *main; 	  // Puntero a la memoria principal
	unsigned delay;   // Retardo de acceso en milisegundos
	unsigned nframes; // Cantidad de marcos
	size_t sframe;    // Tamaño de cada marco
} memory;

void init() {
	memory.main = alloc(config->marcos * config->marco_size * sizeof(byte));
	memory.delay = config->retardo_memoria;
	memory.nframes = config->marcos;
	memory.sframe = config->marco_size;
	cache_create(config->entradas_cache, config->marco_size, config->cache_x_proc);
	server_start(config->puerto);
}

void terminate() {
	server_end();
	cache_destroy();
	free(config);
	free(memory.main);
	puts("Proceso Memoria finalizado con éxito");
}

int main(int argc, char **argv) {
    guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	config = get_config(argv[1]);

	set_current_process(MEMORY);
	title("MEMORIA");

	init();
	console();
	terminate();

	return EXIT_SUCCESS;
}

unsigned memory_get_access_time() {
	return memory.delay;
}

void memory_set_access_time(unsigned delay) {
	memory.delay = delay;
}

unsigned memory_get_frame_number() {
	return memory.nframes;
}

size_t memory_get_frame_size() {
	return memory.sframe;
}

void memory_flush_cache() {
	cache_flush();
}
