# tp-2017-1c-System-Buzz

## Prerequisitos

* Instalar SO Commons Library 
	- https://github.com/sisoputnfrba/so-commons-library
* Instalar ansisop-parser 
	- https://github.com/sisoputnfrba/ansisop-parser
	
Los archivos de configuracion se indican como argumentos de linea de comando. Para ejecutar un proceso correctamente pararse en la carpeta Debug del mismo y ejecutar `./Proceso ../metadata` (o seguir leyendo para ver cómo ejecutar procesos más fácilmente).

## Script para facilitar la ejecución de los procesos

Ventajas de correr los procesos usando este script:
* No tenés que acordarte donde está la carpeta del proyecto y hacer `cd` hasta encontrar el binario. Con el script podés ejecutarlo desde cualquier lado.
* En lugar de, por ejemplo, ejecutar el proceso Consola haciendo `./Consola ../metadata`, solo hay que hacer `so term` y el script se encarga de pasarle el archivo de configuración.
* Si hubo cambios en el código fuente, el script usa el makefile de Eclipse para recompilar el binario antes de ejecutarlo.
* Se puede ver el log de un determinado proceso haciendo `so {proceso} --log`. Si no se especifica ningún proceso, el script muestra todos los logs juntos.

### Instrucciones de instalación

1) Bajar el script de acá: https://www.dropbox.com/s/h5wrtqem8rouxy3/so.sh?dl=1
2) Cambiar la ruta del proyecto a la que corresponda en tu sistema (línea 3, variable PROJECT)
3) Ir a una terminal, moverse con `cd` hasta donde está el script y ejecutar lo siguiente:

```bash
chmod +x so.sh && sudo mv so.sh /usr/local/bin/so
```

¡Listo! Ahora podés hacer **so term** para correr la _Consola_, **so cpu** para el _CPU_, **so mem** para la _Memoria_, **so kern** para el _Kernel_ y **so fs** para el _File System_.
