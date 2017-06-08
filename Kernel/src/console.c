#include "console.h"
#include "Kernel.h"

void terminal() {
    title("Consola");

    while(true) {
            char *argument = input(command);
            if(streq(command, "processlist")) {
                for (int i = 0; i < pcb_exec->elements_count; i++){
                    t_pcb* aux = list_get (pcb_exec, i);
                    printf("PID: %d\n", aux->idProcess);
                }
                for (int i = 0; i < pcb_exit->elements_count; i++){
                    t_pcb* aux = list_get (pcb_exit, i);
                    printf("PID: %d\n", aux->idProcess);
                }
                for (int i = 0; i < pcb_new->elements_count; i++){
                    t_pcb* aux = list_get (pcb_new, i);
                    printf("PID: %d\n", aux->idProcess);
                }
                for (int i = 0; i < pcb_block->elements_count; i++){
                    t_pcb* aux = list_get (pcb_block, i);
                    printf("PID: %d\n", aux->idProcess);
                }
                for (int i = 0; i < pcb_ready->elements_count; i++){
                    t_pcb* aux = list_get (pcb_ready, i);
                    printf("PID: %d\n", aux->idProcess);
                }
            } else if(streq(command, "status")) {
                int pid = atoi(argument);
                                for (int i = 0; i < pcb_exec->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_exec, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "EXECUTING");
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_exit->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_exit, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "EXIT");
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_new->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_new, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "NEW");
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_block->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_block, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "BLOCK");
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_ready->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_ready, i);
                                    if (aux->idProcess == pid){
                                        printf("Status PID %d: %s\n", pid, "READY");
                                        break;
                                    }
                                }
            } else if(streq(command, "kill")) {
                int pid = atoi(argument);
                bool stop = 0;
                    while (!stop){
                                for (int i = 0; i < pcb_new->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_new, i);
                                    if (aux->idProcess == pid){
                                        aux = list_remove (pcb_new, i);
                                        aux->exitCode = -2;
                                        aux->status = EXIT;
                                        list_add (pcb_exit, aux);
                                        stop = 1;
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_block->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_block, i);
                                    if (aux->idProcess == pid){
                                        aux = list_remove (pcb_block, i);
                                        aux->exitCode = -2;
                                        aux->status = EXIT;
                                        list_add (pcb_exit, aux);
                                        stop = 1;
                                        break;
                                    }
                                }
                                for (int i = 0; i < pcb_ready->elements_count; i++){
                                    t_pcb* aux = list_get (pcb_ready, i);
                                    if (aux->idProcess == pid){
                                        aux = list_remove (pcb_ready, i);
                                        aux->exitCode = -2;
                                        aux->status = EXIT;
                                        list_add (pcb_exit, aux);
                                        stop = 1;
                                        break;
                                    }
                                }
                    }
            } else if(streq(command, "stop")) {
            	pthread_mutex_lock(&mutex_planificacion);
            } else if(streq(command, "restart")) {
            	pthread_mutex_unlock(&mutex_planificacion);
            }else if(streq(command, "help")){
                                puts("processlist	-Muestra todos los procesos que esta manejando el Kernel");
                                puts("status PID	-Muestra el estado en el que se encuentra el proceso");
                                puts("kill PID	-Finaliza el proceso correspondiente al PID ingresado");
                                puts("stop		-Detiene la planificacion de los procesos");
                                puts("restart	-Inicia la planificacion de los procesos detenida");
                                puts("help		-Muestra las opciones de ayuda");
                       }else{
                           puts("Comando no reconocido. Escriba 'help' para ayuda.");
                       }
                   }
           }
