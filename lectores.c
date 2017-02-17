/*
 * Ejercicio 4 - Proceso lectores
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

int N1, N2;

struct parameters {
    int idLector;
};

sem_t sem_lectura_llena;
sem_t sem_lectura_entrar[40]; //Hay que declararlos fuera para que la otra función tenga acceso a ellos y evitar enviarlo por referencia.
sem_t sem_lectura_salir[40];

void* lector(struct parameters*);

int main(int charc, char *argv[]) {
    char c;

    if (charc != 3) {
        printf("El número de parámetros introducido no es correcto. Saliendo. \n");
        exit(-22);
    }
    if(atoi(argv[1]) <= atoi(argv[2])){
        printf("N1 no puede ser menor que N2. Saliendo.\n");
        exit(-22);
    }

    N1 = atoi(argv[1]); //Número total de lectores posible
    N2 = atoi(argv[2]); //Número total de lectores simultáneos
    pthread_t thread[N1];
    struct parameters param[N1];
    sem_init(&sem_lectura_llena, 0, N2); //semáforo para delimitar la zona de exclusión

    /*
     * Creación de los hilos
     */

    for (int i = 1; i <= N1; i++) {
        param[i].idLector = i;
        sem_init(&sem_lectura_entrar[i], 0, 0);
        sem_init(&sem_lectura_salir[i], 0, 0);
        if (pthread_create(&thread[i], NULL, (void *)lector, (void *) &param[i])) {
            perror("Error creando hilo lector. \n");
            exit(-1);
        }
    }

    /*
     * Menú
     */

    while(1) {
        printf("Introduzca una acción: \n");
        printf("\t 1. Intentar leer.\n");
        printf("\t 2. Finalizar leer.\n");
        printf("\t 3. Salir.\n");
        int opt = 0;
        scanf("%d", &opt);
        switch (opt) {
            case 1 :
                printf("\n\t>introduzca el número del lector (de 1 a %i).\n", N1);
                scanf("%d", &opt);
                sem_post(&sem_lectura_entrar[opt]);
                break;
            case 2 :
                printf("\n\t>introduzca el número del lector (de 1 a %i)\n", N1);
                scanf("%d", &opt);
                sem_post(&sem_lectura_salir[opt]);
                break;
            case 3 :
                printf("\nHa seleccionado salir. Saliendo.\n");
                for (int i = 1; i <= N1; i++) {
                    pthread_cancel(thread[i]);
                    sem_close(&sem_lectura_salir[i]);
                    sem_close(&sem_lectura_entrar[i]);
                    sem_close(&sem_lectura_llena);
                }
                pthread_exit(NULL);
                break;
            default :
                printf("\nInstrucción no válida. Vuelva a intentarlo.\n");
                break;
        }
    }
}

void* lector(struct parameters* param) {
    int myId = param->idLector;
    printf("[Lector %i] -­‐‑> Iniciado\n", myId);
    while(1){
        printf("[Lector %i] -­‐‑> Esperando a intentar leer…\n", myId);
        sem_wait(&sem_lectura_entrar[myId]);
        //Me indican que puedo leer.
        printf("[Lector %i] -­‐‑> Intentando leer…\n", myId);
        sem_wait(&sem_lectura_llena);
        //Consigo acceso
        //Leo
        printf("[Lector %i] -­‐‑> Leyendo\n", myId);
        sem_wait(&sem_lectura_salir[myId]);
        //Me ordenan terminar de leer
        printf("[Lector %i] -­‐‑> Fin lectura\n", myId);
        sem_post(&sem_lectura_llena);
    }
    pthread_exit(NULL);
}
