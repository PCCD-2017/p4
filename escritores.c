/*
 * Ejercicio 5 - Proceso escritores
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>
#include <unistd.h>

struct parameters {
    int idEscritor;
};

/**
 * Menu principal.
 * @return 1 a N3 o 0 si se va a salir del programa.
 */
int menu();

/**
 * Thread.
 * @param id del thread.
 */
void* escritor(struct parameters* param);

/**
 * Variables globales.
 */
int N3;
sem_t sem_via_principal;
sem_t sem_calle[50];
sem_t sem_salida;

/**
 * Inicio del programa.
 * @param argc
 * @param argv
 * @return 0
 */
int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("\nError al introducir los datos. Saliendo. \n");
        exit(-1);
    }
    N3 = atoi(argv[1]);
    pthread_t  pthread[N3];
    struct parameters param[N3];
    sem_init(&sem_via_principal,0,1);
    sem_init(&sem_salida,0,0);
    /**
    * @Note:
    * @sem_init: int sem_init(sem_t *sem, int pshared, unsigned int value);
    * Values:
    * @sem: points to a semaphore object to initialize.
    * @pshared: is a flag indicating whether or not the semaphore should be
    * shared with fork()ed processes.
    * @value: is an initial value to set the semaphore to.
    * */
    for (int i = 1; i <= N3; i++) {
        param[i].idEscritor = i;
        sem_init(&sem_calle[i],0,0);
        if (pthread_create(&pthread[i], NULL, (void *)escritor,&param[i])) {
            perror("Error creando hilo lector. \n");
            exit(-1);
        }
    }
    sleep(1);
    while (menu()!=0) sleep(1);
    return 0;
}

int menu(){
    int opcion;
    printf("\n\n****************** Menu principal ******************");
    printf("\nPulse 1 para: Intentar escribir.");
    printf("\nPulse 2 para: Finalizar escribir.");
    printf("\nPulse 3 para: Salir.");
    printf("\n****************************************************");
    printf("\n\nOpcion: ");
    scanf("%d",&opcion);
    switch (opcion){
        case 1:
            printf("\n\nHa seleccionado la opcion 1 (Intentar escribir).");
            printf("\n >Introduzca el numero del escritor (de 1 a %d).",N3);
            scanf("%d",&opcion);
            sem_post(&sem_calle[opcion]);
            break;
        case 2:
            printf("\n\nHa seleccionado la opcion 2 (Finalizar escribir).");
            printf("\n >Introduzca el numero del lector (de 1 a %d).",N3);
            scanf("%d",&opcion);
            sem_post(&sem_salida);
            break;
        case 3:
            printf("\n\nSaliendo...");
            opcion = 0;
            break;
        default:
            printf("\nOpcion no valida, Saliendo del programa.");
            opcion = 0;
            break;
    }
    return opcion;
}

void* escritor(struct parameters* param) {
    int idEscritor = param->idEscritor;
    while (1){
        printf("\n[Escritor %d] -> Esperando a intentar escribir.",idEscritor);
        printf("\n[Escritor %d] -> Intentando escribir...\n",idEscritor);
        sem_wait(&sem_calle[idEscritor]);
        sem_wait(&sem_via_principal);
        printf("\n[Escritor %d] -> Escribiendo...",idEscritor);
        sem_wait(&sem_salida);
        printf("\n[Escritor %d] -> Fin escritura.",idEscritor);
        sem_post(&sem_via_principal);
    }
}