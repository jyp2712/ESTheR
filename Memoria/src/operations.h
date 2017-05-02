#ifndef operations_h
#define operations_h

/**
 * Cuando el Proceso Kernel comunique el inicio de un nuevo Programa, se crearán las estructuras
 * necesarias para administrarlo correctamente. En una misma página no se podrán tener datos
 * referentes a dos segmentos diferentes (por ej. Código y Stack, o Stack y Heap).
 * @param pid ID del proceso de usuario.
 * @param npages Número de páginas requeridas por el proceso.
 */
void alloc_program(int pid, int npages);

/**
 * Ante un pedido de lectura de página de alguno de los procesos CPU, se realizará la traducción
 * a marco (frame) y se devolverá el contenido correspondiente consultando primeramente a la
 * Memoria Caché. En caso de que esta no contenga la info, se informará un Cache Miss, se deberá
 * cargar la página en Caché y se devolverá la información solicitada.
 * @param ID del proceso de usuario.
 * @param pageno Número de página solicitada.
 * @param offset Desplazamiento dentro de la págine.
 * @param size Tamaño del bloque de memoria solicitado.
 * @return Bloque de memoria solicitado.
 */
byte *read_from_page(int pid, int pageno, int offset, int size);

/**
 * Ante un pedido de escritura de página de alguno de los procesadores, se realizará la traducción
 * a marco (frame), y se actualizará su contenido. En caso de que la página se encuentre en Memoria
 * Caché, se deberá actualizar también el frame alojado en la misma.
 * @param pid ID del proceso de usuario.
 * @param pageno Número de página a escribir.
 * @param offset Desplazamiento dentro de la página.
 * @param size Tamaño del bloque a escribir.
 * @param buffer Búfer con los datos a escribir.
 */
void write_to_page(int pid, int pageno, int offset, int size, byte *buffer);

/**
 * Ante un pedido de asignación de páginas por parte del kernel, el proceso memoria deberá asignarle
 * tantas páginas como se soliciten al proceso ampliando así su tamaño. En caso de que no se pueda
 * asignar más páginas se deberá informar de la imposibilidad de asignar por falta de espacio.
 * @param pid ID del proceso de usuario.
 * @param npages Número de páginas requeridas por el proceso.
 */
void realloc_program(int pid, int npages);

/**
 * Cuando el Proceso Kernel informe el fin de un Programa, se deberán eliminar las entradas en
 * estructuras usadas para administrar la memoria.
 * @param pid ID del proceso de usuario.
 */
void free_program(int pid);


#endif /* operations_h */
