/*
 * Ejercicio 2
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

struct parameters{
    int pos;
    char* charAtPos;
};

void* imprimir_hilo(struct parameters*);

int main(int argc, char* argv[]){
    pthread_t thread[argc - 1];
    int i;
    struct parameters param[argc - 1];
    for(i = 0; i < argc - 1; i++){
        param[i].pos = i + 1;
        param[i].charAtPos = argv[i + 1];
        if(pthread_create(&thread[i], NULL, (void*)imprimir_hilo, &param[i])){
            /*
             * The last parameter is important here: the actual parameter at the given position.
             */
            printf("No se ha podido crear un hilo\n");
            return 0;
        }
    }
    pthread_exit(NULL);
}

void* imprimir_hilo(struct parameters* param){
    printf("Parametro %d: %s\n", param -> pos, param -> charAtPos);
    pthread_exit(NULL);
}
