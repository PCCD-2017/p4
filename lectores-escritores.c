#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/** estructura para lectores donde almacenamos ID*/
struct lectorParam {
    int idLector;
};

/** estructura para escritores donde almacenamos ID*/
struct escritorParam {
    int idEscritor;
};

/**
 * Menu principal.
 * @return 1 a N5 o 0 si se va a salir del programa.
 */
int menu();

/** Funcion para procesos lectores*/
void* lector(struct lectorParam* param);

/** Funcion para procesos escritores*/
void* escritor(struct escritorParam* param);

/** Variables globales.*/
int N3,N2,N1;

/**Semaforos para sincronizacion entre escritores.*/
sem_t sem_via_principal_escritor;
sem_t sem_calle_escritor[50];
sem_t sem_salida_escritor;

/**Semaforos para sincronizacion entre lectores.*/
sem_t sem_lectura_llena_lector;
sem_t sem_lectura_entrar_lector[40]; //Hay que declararlos fuera para que la otra función tenga acceso a ellos y evitar enviarlo por referencia.
sem_t sem_lectura_salir_lector[40];

/**Semaforos para sincronizacion entre lectores y escritores.*/
sem_t escritor_zona_critica;
sem_t lector_zona_critica;

int main(int argc, char *argv[]){
    printf("entra");
    if (argc != 4) {
        printf("\nError al introducir los datos. Saliendo. \n");
        exit(-1);
    }

    /** Lectores. */
    N1 = atoi(argv[1]); //Número total de lectores posible
    N2 = atoi(argv[2]); //Número total de lectores simultáneos
    pthread_t thread_lector[N1];
    struct lectorParam struct_lector[N1];
    sem_init(&sem_lectura_llena_lector, 0, N2); //semáforo para delimitar la zona de exclusión
    for (int i = 1; i <= N1; i++) {
        struct_lector[i].idLector = i;
        sem_init(&sem_lectura_entrar_lector[i], 0, 0);
        sem_init(&sem_lectura_salir_lector[i], 0, 0);
        if (pthread_create(&thread_lector[i], NULL, (void *)lector, &struct_lector[i])) {
            perror("Error creando hilo lector. \n");
            exit(-1);
        }
    }

    /** Escritores */
    N3 = atoi(argv[3]);
    pthread_t  pthread_escritor[N3];
    struct escritorParam struct_escritor[N3];
    sem_init(&sem_via_principal_escritor,0,1);
    sem_init(&sem_salida_escritor,0,0);
    for (int i = 1; i <= N3; i++) {
        struct_escritor[i].idEscritor = i;
        sem_init(&sem_calle_escritor[i],0,0);
        if (pthread_create(&pthread_escritor[i], NULL, (void *)escritor,&struct_escritor[i])) {
            perror("Error creando hilo lector. \n");
            exit(-1);
        }
    }

    /** Semaforos de control de zona critica*/
    sem_init(&escritor_zona_critica,0,0);
    sem_init(&lector_zona_critica,0,0);

    /** Menu principal*/
    sleep(1);
    while (menu()!=0) sleep(1);

    return 0;
}

int menu(){
    int opcion;
    printf("\n\n****************** Menu principal ******************");
    printf("\nPulse 1 para: Intentar escribir.");
    printf("\nPulse 2 para: Finalizar escribir.");
    printf("\nPulse 3 para: Intentar leer.");
    printf("\nPulse 4 para: Finalizar leer.");
    printf("\nPulse 5 para: Salir.");
    printf("\n****************************************************");
    printf("\n\nOpcion: ");
    scanf("%d",&opcion);
    switch (opcion){
        case 1:
            printf("\n >Introduzca el numero del escritor (de 1 a %d).",N3);
            scanf("%d",&opcion);
            sem_post(&sem_calle_escritor[opcion]);
            sem_post(&escritor_zona_critica);
            break;
        case 2:
            printf("\n >Introduzca el numero del escritor (de 1 a %d).",N3);
            scanf("%d",&opcion);
            sem_post(&sem_salida_escritor);
            break;
        case 3:
            printf("\n >Introduzca el número del lector (de 1 a %i).\n", N1);
            scanf("%d", &opcion);
            sem_post(&sem_lectura_entrar_lector[opcion]);
            sem_post(&lector_zona_critica);
            break;
        case 4:
            printf("\n >Introduzca el número del lector (de 1 a %i)\n", N1);
            scanf("%d", &opcion);
            sem_post(&sem_lectura_salir_lector[opcion]);
            break;
        case 5:
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

void* escritor(struct escritorParam* param){
    int idEscritor = param->idEscritor;
    while (1){
        printf("\n[Escritor %d] -> Esperando a intentar escribir.",idEscritor);
        printf("\n[Escritor %d] -> Intentando escribir...\n",idEscritor);
        sem_wait(&sem_calle_escritor[idEscritor]);
        sem_wait(&sem_via_principal_escritor);
        sem_wait(&escritor_zona_critica);
        printf("\n[Escritor %d] -> Escribiendo...",idEscritor);
        sem_wait(&sem_salida_escritor);
        printf("\n[Escritor %d] -> Fin escritura.",idEscritor);
        sem_post(&sem_via_principal_escritor);
        sem_post(&escritor_zona_critica);
    }
}

void* lector(struct lectorParam* param){
    int myId = param -> idLector;
    printf("[Lector %i] -­‐‑> Iniciado\n", myId);
    while(1){
        printf("[Lector %i] -­‐‑> Esperando a intentar leer…\n", myId);
        sem_wait(&sem_lectura_entrar_lector[myId]);
        //Me indican que puedo leer.
        printf("[Lector %i] -­‐‑> Intentando leer…\n", myId);
        sem_wait(&sem_lectura_llena_lector);
        sem_wait(&lector_zona_critica);
        //Consigo acceso
        //Leo
        printf("[Lector %i] -­‐‑> Leyendo\n", myId);
        sem_wait(&sem_lectura_salir_lector[myId]);
        //Me ordenan terminar de leer
        printf("[Lector %i] -­‐‑> Fin lectura\n", myId);
        sem_post(&sem_lectura_llena_lector);
        sem_post(&lector_zona_critica);
    }
}