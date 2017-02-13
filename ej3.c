/*
 * Ejercicio 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

void* Carvajal(void*);
void* Ramos(void*);
void* Pique(void*);
void* Iniesta_silva(void*);

int main(){
    pthread_t carvajal,ramos,pique,iniesta_silva;
    printf("De Gea ");
    pthread_create(&carvajal,NULL,(void*)Carvajal,NULL);
    pthread_create(&ramos,NULL,(void*)Ramos,NULL);
    pthread_create(&pique,NULL,(void*)Pique,NULL);
    printf("Jordi-Alba ");
    pthread_create(&iniesta_silva,NULL,(void*)Iniesta_silva,NULL);
    pthread_join(carvajal,NULL);
    pthread_join(ramos,NULL);
    pthread_join(pique,NULL);

    /*
     * From the manual:
     * pthread_join  suspends  the  execution  of the calling thread until
     * the thread identified by th terminates, either by  calling  pthread_exit(3)
     * or by being cancelled.
     */

    printf("Busquets ");
    printf("Nolito ");
    printf("Isco ");
    pthread_join(iniesta_silva,NULL);
    printf("Aspas\n");
    pthread_exit(NULL);
    return 0;
}

void* Carvajal(void* parameters){
    printf("Carvajal ");
    pthread_exit(NULL);
}
void* Ramos(void* parameters){
    printf("Ramos ");
    pthread_exit(NULL);
}
void* Pique(void* parameters){
    printf("Pique ");
    pthread_exit(NULL);
}
void* Iniesta_silva(void* parameters){
    printf("Iniesta ");
    printf("Silva ");
    pthread_exit(NULL);
}
