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
size_t serial_pack_pcb (t_pcb*, unsigned char*);
size_t serial_pack_stack (t_list* stack, unsigned char* buff);
size_t serial_pack_vars (t_list* lista, unsigned char* buff);
size_t serial_pack_retPos (t_list* lista, unsigned char* buff);


/*
 * Deserializa datos según el formato especificado.
 * @param buf Búfer de datos serializados.
 */
void serial_unpack(unsigned char *buf, char *format, ...);
void serial_unpack_pcb (t_pcb*, unsigned char*);
void serial_unpack_stack (t_list* stack, unsigned char* buff);
size_t serial_unpack_vars (t_list* lista, unsigned char* buff);
size_t serial_unpack_retPos (t_list* lista, unsigned char* buff);

#endif /* serial_h */
