/*
 * File_System.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

typedef struct{
	char* puerto;
	char* punto_montaje;
}t_file_system;

void leerConfiguracionFileSystem(t_file_system* file_system, char* path);

#endif /* FILE_SYSTEM_H_ */
