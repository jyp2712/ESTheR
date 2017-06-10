#include "hash_table.h"
#include "utils.h"
#include "Memoria.h"
#include "configuration.h"
#include "globals.h"
#include "utils.h"
#include "commons/collections/queue.h"

#define incorrect -1


t_element_hash *hash_table;
int CANT_MAX_FRAME;


void create_table_hash(int MARCOS){
	CANT_MAX_FRAME=MARCOS;
	hash_table=alloc(sizeof(t_element_hash)*CANT_MAX_FRAME);
	for(int i=0;i<CANT_MAX_FRAME;i++){
		hash_table[i].id_Process=(int)NULL;
		hash_table[i].page=(int)NULL;
	}

}

int add_element(int pag,int id_Pro){

	int final_arrays=true;
	t_element_hash *element_hash = alloc(sizeof(t_element_hash));

	element_hash->page=pag;
	element_hash->id_Process=id_Pro;


	if(table_complete(hash_table)){
		puts("Se completo la tabla de Frames");
		return incorrect;
	}else {
		int keys=create_keys(element_hash->page,element_hash->id_Process);
		if(hash_table[keys].page==(int)NULL){
			hash_table[keys]=*element_hash;
		}else{
			for(int i=keys+1;i<CANT_MAX_FRAME;i++){
				if(hash_table[keys].page==(int)NULL){
					hash_table[i]=*element_hash;
					final_arrays=false;
					return i;
				}

			}if(final_arrays==true){
				for(int i=0;i<keys;i++){
					hash_table[i]=*element_hash;
					return i;

				}
			}

		}
	}
}


int create_keys(int page,int id_Process){
	char str1[15];
	char str2[15];
	sprintf(str1, "%d", id_Process);
	sprintf(str2, "%d", page);
	strcat(str1,str2);
	unsigned int keys=atoi(str1)%CANT_MAX_FRAME;
	return keys;

}


bool table_complete(){
	int final_arrays=final;
	for(int i;i<CANT_MAX_FRAME;i++){
		if(hash_table[i].page==(int)NULL){
			final_arrays=no_final;
			return final_arrays;

		}

	}
	return final_arrays;
}

int element_search(int page, int id_Process){
	int keys=create_keys(page,id_Process);
	if(hash_table[keys].id_Process==id_Process && hash_table[keys].page==page){
		return keys;}
	else{for(int i=keys+1; i<CANT_MAX_FRAME;i++){//busca desde la clave hasta final de array
			if(hash_table[i].id_Process==id_Process && hash_table[i].page==page){
				return i;
			}
		}for(int i=0;i<keys;i++){//busca desde inicio hasta la clave
			if(hash_table[i].id_Process==id_Process && hash_table[i].page==page)
			     return i;

		 }

	  }
	return incorrect;

}


bool delete_element(int page,int id_Process){
	int indice=element_search(page,id_Process);
	if(indice==incorrect){
		return false;
	}else{
		hash_table[indice].id_Process=(int)NULL;
		hash_table[indice].page=(int)NULL;

	}
	return true;

}
