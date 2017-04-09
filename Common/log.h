#ifndef log_h
#define log_h

#define LOG_ENABLED true
#define LOG_STDOUT true

/**
 * Informa un mensaje en el archivo de log del proceso en ejecución.
 * @param format Cadena con formato indicando el mensaje de log.
 */
void log_inform(const char *format, ...);

/**
 * Reporta un problema en el archivo de log del proceso en ejecución.
 * @param format Cadena con formato indicando el mensaje de log.
 */
void log_report(const char *format, ...);

#endif /* log_h */
