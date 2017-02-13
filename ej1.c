/*
 * Ejercicio 1
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* funcion_hilo(void*);
char c = 'a';
int counter = 0;

int main (int argc, char* argv[]){
    pthread_t hilo;

    if(pthread_create(&hilo, NULL, funcion_hilo, NULL)){
        /*
         * int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);
            Params:
                -> *thread : an empty thread where all the information from the new one will be stored.
                -> *attr : From the man :
                    "The attr argument points to a pthread_attr_t structure whose contents
                     are used at thread creation time to determine attributes for the new
                     thread; this structure is initialized using pthread_attr_init(3) and
                     related functions.  If attr is NULL, then the thread is created with
                     default attributes."
                -> *(start_routine) : Points to the routine on which the thread may start. It must return void* and receive void*.
                    Source: http://timmurphy.org/2010/05/04/pthreads-in-c-a-minimal-working-example/
                -> *arg : From the man:
                    "arg is passed as the sole argument of start_routine()."
         */
        perror("Error en pthread create: ");
        return -1;
    }

    while(c != 'q'){
        sleep(1);
        printf("Numero de caracteres leidos del teclado: %d\n", counter);
    }
    printf("Ha pulsado q. Saliendo.\n");
    return 0;
}

void* funcion_hilo (void* parameter){
    do{
        printf("Pulse 'q' + <ENTER> para salir.\n");
        fscanf(stdin,"%c",&c);
        counter++;
    }while(c != 'q');
    return 0;
}
