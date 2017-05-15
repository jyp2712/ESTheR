#ifndef serial_h
#define serial_h
#include "structures.h"
#include "utils.h"

/*
 * Tabla de referencia para el formato de serialización
 *
 *   bits |signed   unsigned   float   string
 *   -----+----------------------------------
 *      8 |   c        C
 *     16 |   h        H         f
 *     32 |   l        L         d
 *     64 |   q        Q         g
 *      - |                               s
 */

/*
 * Serializa datos según el formato especificado.
 * @param buf Búfer de datos serializados.
 * @return Tamaño de los datos serializados.
 */
size_t serial_pack(unsigned char *buf, char *format, ...);


/*Serialización del pcb.
 *
 * pcb : El puntero al pcb que se quiere serializar.
 * buff : El buffer donde se almacenaran los datos del pcb.
 */
size_t serial_pack_pcb (t_pcb* pcb, unsigned char* buff);

/*Serializa las entradas del stack en una char*.
 *
 * stack  : El stack que se quiere serializar.
 * buff : El buffer donde se quiere almacenar el stack serializado.
 */
size_t serial_pack_stack (t_list* stack, unsigned char* buff);
size_t serial_pack_vars (t_list* lista, unsigned char* buff);


/*
 * Deserializa datos según el formato especificado.
 * @param buf Búfer de datos serializados.
 */
void serial_unpack(unsigned char *buf, char *format, ...);

/*Deserializa el pcb.
 *
 * pcb : El puntero al pcb que se habra deserializado.
 * buff : El buffer donde estaran almacenados los datos del pcb.
 */
void serial_unpack_pcb (t_pcb* pcb, unsigned char* buff);

/*Deserializa un t_stack de un conjunto de bytes.
 *
 * stack    : Donde el stack se almacenara.
 * stack : Conjunto de datos serializados.
 */
void serial_unpack_stack (t_list* stack, unsigned char* buff);
size_t serial_unpack_vars (t_list* lista, unsigned char* buff);

#endif /* serial_h */
