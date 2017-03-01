#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>

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
sem_t sem_espaldas[100];
sem_t acceso_numlectores;
sem_t acceso_numescritores;
sem_t acceso_leyendo;
sem_t mod_espalda;
sem_t paso_escritores;

/**Semaforos para sincronizacion entre lectores y escritores.*/
sem_t sem_excl_mutua;

int* dir_num_lectores;
int* dir_leyendo;
int* dir_espalda;
int* dir_num_escritores;

int* quiere_leer;
int* adelantamientos;
int* num_max_adelant;

int main(int argc, char *argv[]){
    key_t key_num_lectores = 88888;
    key_t key_leyendo = 99999;
    key_t key_espalda = 999888;
    key_t key_num_escritores = 888999;
    key_t key_quiere_leer = 777777;
    key_t key_adelantamiento = 666666;
    key_t key_num_max_adelant = 777666;
    int shmid;
    if (argc != 5) {
        printf("\nError al introducir los datos. Saliendo. \n");
        exit(-1);
    }
    if ((shmid = shmget(key_num_lectores, sizeof(int), IPC_CREAT)) == -1) {
        perror("Hubo un error al ejecutar shmget para numlectores.");
        exit(1);
    } else {
        printf("Shmget devolvió %d .\n", shmid);
        if((dir_num_lectores = shmat(shmid, NULL, 0)) == (int *) -1){
            perror("Hubo un error al ejecutar shmat para numlectores.");
            exit(1);
        }
    }
    if ((shmid = shmget(key_leyendo, sizeof(int), IPC_CREAT)) == -1) {
        perror("Hubo un error al ejecutar shmget para leyendo.");
        exit(1);
    } else {
        printf("Shmget devolvió %d .\n", shmid);
        if((dir_leyendo = shmat(shmid, NULL, 0)) == (int *)-1){
            perror("Hubo un error al ejecutar shmat para leyendo.");
            exit(1);
        }
    }
    if ((shmid = shmget(key_espalda, sizeof(int), IPC_CREAT)) == -1) {
        perror("Hubo un error al ejecutar shmget para leyendo.");
        exit(1);
    } else {
        printf("Shmget devolvió %d .\n", shmid);
        if((dir_espalda = shmat(shmid, NULL, 0)) == (int *) -1){
            perror("Hubo un error al ejecutar shmat para leyendo.");
            exit(1);
        }
    }

    if ((shmid = shmget(key_num_escritores, sizeof(int), IPC_CREAT)) == -1) {
        perror("Hubo un error al ejecutar shmget para leyendo.");
        exit(1);
    } else {
        printf("Shmget devolvió %d .\n", shmid);
        if((dir_num_escritores = shmat(shmid, NULL, 0)) ==(int *) -1){
            perror("Hubo un error al ejecutar shmat para leyendo.");
            exit(1);
        }
    }

    if ((shmid = shmget(key_quiere_leer, sizeof(int), IPC_CREAT)) == -1) {
        perror("Hubo un error al ejecutar shmget para leyendo.");
        exit(1);
    } else {
        printf("Shmget devolvió %d .\n", shmid);
        if((quiere_leer = shmat(shmid, NULL, 0)) == (int *) -1){
            perror("Hubo un error al ejecutar shmat para leyendo.");
            exit(1);
        }
    }

    if ((shmid = shmget(key_adelantamiento, sizeof(int), IPC_CREAT)) == -1) {
        perror("Hubo un error al ejecutar shmget para leyendo.");
        exit(1);
    } else {
        printf("Shmget devolvió %d .\n", shmid);
        if((adelantamientos = shmat(shmid, NULL, 0)) == (int *) -1){
            perror("Hubo un error al ejecutar shmat para leyendo.");
            exit(1);
        }
    }

    if ((shmid = shmget(key_num_max_adelant, sizeof(int), IPC_CREAT)) == -1) {
        perror("Hubo un error al ejecutar shmget para leyendo.");
        exit(1);
    } else {
        printf("Shmget devolvió %d .\n", shmid);
        if((num_max_adelant = shmat(shmid, NULL, 0)) == (int *) -1){
            perror("Hubo un error al ejecutar shmat para leyendo.");
            exit(1);
        }
    }

    *dir_leyendo= 0;
    *dir_num_lectores = 0;
    *dir_espalda = 0;
    *dir_num_escritores = 0;
    *quiere_leer = 0;
    *adelantamientos = 0;
    *num_max_adelant = atoi(argv[4]); //Numero maximo de adelantamientos

    sem_init(&acceso_leyendo, 0, 1);
    sem_init(&acceso_numlectores, 0, 1);
    sem_init(&mod_espalda, 0, 1);
    sem_init(&acceso_numescritores,0,1);
    sem_init(&paso_escritores,0,0);

    /** Lectores. */
    N1 = atoi(argv[1]); //Número total de lectores posible
    N2 = atoi(argv[2]); //Número total de lectores simultáneos
    pthread_t thread_lector[N1];
    struct lectorParam struct_lector[N1];
    //semáforo para delimitar la zona de exclusión
    sem_init(&sem_lectura_llena_lector, 0, (unsigned int) N2);
    for (int i = 1; i <= N1; i++) {
        struct_lector[i].idLector = i;
        sem_init(&sem_lectura_entrar_lector[i], 0, 0);
        sem_init(&sem_lectura_salir_lector[i], 0, 0);
        sem_init(&sem_espaldas[i], 0, 0);
        if (pthread_create(&thread_lector[i], NULL, (void *)lector, &struct_lector[i])) {
            perror("Error creando hilo lector. \n");
            exit(-1);
        }
    }

    /** Escritores */
    N3 = atoi(argv[3]); //Número totoal de escritores posible
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
    sem_init(&sem_excl_mutua, 0, 1);

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
            break;
        case 2:
            sem_post(&sem_salida_escritor);
            break;
        case 3:
            printf("\n >Introduzca el número del lector (de 1 a %i).\n", N1);
            scanf("%d", &opcion);
            sem_post(&sem_lectura_entrar_lector[opcion]);
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
    int primero = 0;
    printf("\n[Escritor %i] -­‐‑> Iniciado\n", idEscritor);
    while (1){
        sem_wait(&sem_calle_escritor[idEscritor]);
        printf("\n[Escritor %d] -> Esperando a intentar escribir.",idEscritor);

        sem_wait(&acceso_numescritores);
        if (*dir_num_escritores == 0) primero = 1;
        *dir_num_escritores = *dir_num_escritores + 1;
        sem_post(&acceso_numescritores);

        sem_wait(&sem_via_principal_escritor);
        printf("\n[Escritor %d] -> Intentando escribir...\n",idEscritor);

        if (*quiere_leer) {
          if (*adelantamientos == *num_max_adelant) {
            sem_post(&sem_excl_mutua);
            sem_wait(&paso_escritores);
          }
        }

        if(primero) sem_wait(&sem_excl_mutua);
        if (*quiere_leer) *adelantamientos = *adelantamientos + 1;
        printf("\n[Escritor %d] -> Escribiendo...",idEscritor);
        printf("\nAdelantamientos = %d",*adelantamientos);
        sem_wait(&sem_salida_escritor);
        printf("\n[Escritor %d] -> Fin escritura.",idEscritor);
        sem_post(&sem_via_principal_escritor);

        sem_wait(&acceso_numescritores);
        if (*dir_num_escritores == 1) sem_post(&sem_excl_mutua);
        *dir_num_escritores = *dir_num_escritores - 1;
        sem_post(&acceso_numescritores);
        primero = 0;
    }
}

void* lector(struct lectorParam* param){
    int myId = param -> idLector;
    int primero = 0;
    int delante = 0;
    printf("\n[Lector %i] -­‐‑> Iniciado\n", myId);
    while(1){
        printf("\n[Lector %i] -­‐‑> Esperando a intentar leer…\n", myId);
        sem_wait(&sem_lectura_entrar_lector[myId]);
        //Me indican que puedo leer.
        printf("\n[Lector %i] -­‐‑> Intentando leer…\n", myId);
        *quiere_leer = 1;
        sem_wait(&sem_lectura_llena_lector);
        //Consigo acceso
        //Gano acceso a papel contra los demás lectores
        sem_wait(&acceso_numlectores);
        if(*dir_num_lectores == 0){
            *dir_num_lectores = 1;
            primero = 1;
            sem_wait(&mod_espalda);
            *dir_espalda = myId;
            sem_post(&mod_espalda);
        }else{
            int q = 0;
            q = *dir_num_lectores;
            q = q + 1;
            *dir_num_lectores = q;
            sem_wait(&mod_espalda);
            delante = *dir_espalda;
            *dir_espalda = myId;
            sem_post(&mod_espalda);
        }
        sem_post(&acceso_numlectores);

        if(primero){
            sem_wait(&sem_excl_mutua);
            *dir_leyendo = 1;
        }else{
            sem_wait(&sem_espaldas[delante]);
        }


        //Leo
        printf("\n[Lector %i] -­‐‑> Leyendo\n", myId);
        sem_post(&sem_espaldas[myId]);
        sem_wait(&sem_lectura_salir_lector[myId]);

        sem_wait(&acceso_numlectores);
        if(*dir_num_lectores == 1){
            *dir_leyendo = 0;
            //Liberamos zona critica
            *quiere_leer = 0;
            *adelantamientos = 0;
            sem_post(&paso_escritores);
            sem_post(&sem_excl_mutua);
        }else{
            int q = 0;
            q = *dir_num_lectores;
            q = q - 1;
            *dir_num_lectores = q;
        }
        sem_post(&acceso_numlectores);
        //Me ordenan terminar de leer
        printf("\n[Lector %i] -­‐‑> Fin lectura\n", myId);
        sem_post(&sem_lectura_llena_lector);
        primero = 0;
    }
}
