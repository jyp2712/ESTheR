#ifndef serial_h
#define serial_h

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

/*
 * Deserializa datos según el formato especificado.
 * @param buf Búfer de datos serializados.
 */
void serial_unpack(unsigned char *buf, char *format, ...);

#endif /* serial_h */
