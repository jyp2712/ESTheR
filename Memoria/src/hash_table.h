/*
 * hash_table.h
 *
 *  Created on: 9/6/2017
 *      Author: utnso
 */

#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include "utils.h"

#define final 0
#define no_final 1

typedef struct{
	int id_Process;
	int page;
}t_element_hash;

void create_table_hash(int MARCOS);

/*Agrega el elemento en la posicion del array que deriva de la clave*/
/*Si esta ocupada, se guarda en la primer posicion que encruenta en forma ascendente*/
/*retorna el valor del indice o -1 si esta lleno la tabla hash*/
int add_element (int page,int id_Process);

/*crea la clave para agregar el elemento a la tabla hash*/
/***Retorna la clavee o -1*/
int create_keys(int page,int id_Process);

bool table_complete();

/*Retorna l indice donde se encuentra el elemento o -1 si no lo encontro*/
int element_search(int page,int id_Process);


bool delete_element(int page,int id_Process);

#endif /* HASH_TABLE_H_ */
