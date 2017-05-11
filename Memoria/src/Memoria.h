/*
 * Memoria.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "socket.h"

unsigned memory_get_access_time();

void memory_set_access_time(unsigned delay);

unsigned memory_get_frame_number();

size_t memory_get_frame_size();

void memory_flush_cache();

#endif /* MEMORIA_H_ */
