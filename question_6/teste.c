#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"
#include <time.h>

#define N

Queue *lista_pronto;

void agenda(){
    printf("AGENDANDO...\n");
    
    for(long i = 0; i < 1000000000; i++);

    int funcao = rand() % 4;

    case

    enqueue(lista_pronto, );
}

void *execucao(void *arg){
    while(1){

    }
}

int main(void){

    pthread_t escalonador;
    lista_pronto = create_queue();

    int rc = pthread_create(&escalonador, NULL, execucao, NULL);
    if(rc){
        printf("erro na criacao do escalonador\n");
        exit(-1);
    }

    rc = pthread_join(escalonador, NULL);
    if(rc){
        printf("erro na execucao do escalonador\n");
        exit(-1);
    }

    pthread_exit(NULL);
    clear(lista_pronto);

    return 0;
}