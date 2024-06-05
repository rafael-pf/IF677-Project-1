#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "queue.h"

#define N 30
#define NUM_FUNC 4
#define BUFFER_SIZE 100

int a = 3;
int b = 2;

int ocupado[N] = {0}; // vetor de ocupacao dos nucleos

char nomes_funcao[NUM_FUNC][51] = { {"Somar"}, {"Subtrair"}, {"Multiplicar"}, {"Dividir"} }; // nomes das funcoes

pthread_t escalonador; //consumidor
pthread_t enfileirador; //produtor
pthread_t nucleos[N]; // nucleos


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t pode_esvaziar = PTHREAD_COND_INITIALIZER; // condicao para esvaziar a fila
pthread_cond_t pode_encher = PTHREAD_COND_INITIALIZER; // condicao para encher a fila

Queue* lista_pronto; // fila de funcoes


void* somar(void* arg) {
    printf("soma: %d\n", a + b);
    int id = *((int*)arg);

    int rc = pthread_detach(nucleos[id]);
    if(rc){
        printf("Falha no detach da thread %d\n", id);
        exit(-1);
    }
    // liberando o nucleo
    pthread_mutex_lock(&mutex2);
    ocupado[id] = 0;
    pthread_mutex_unlock(&mutex2);


    free(arg);
    pthread_exit(NULL);
}

void* subtrair(void* arg) {
    printf("subtracao: %d\n", a - b);

    int id = *((int*)arg);

    int rc = pthread_detach(nucleos[id]);
    if(rc){
        printf("Falha no detach da thread %d\n", id);
        exit(-1);
    }
    // liberando o nucleo
    pthread_mutex_lock(&mutex2);
    ocupado[id] = 0;
    pthread_mutex_unlock(&mutex2);


    free(arg);
    pthread_exit(NULL);
}

void* multiplicar(void* arg) {
    printf("produto: %d\n", a * b);

    int id = *((int*)arg);

    int rc = pthread_detach(nucleos[id]);
    if(rc){
        printf("Falha no detach da thread %d\n", id);
        exit(-1);
    }
    // liberando o nucleo
    pthread_mutex_lock(&mutex2); 
    ocupado[id] = 0;
    pthread_mutex_unlock(&mutex2);


    free(arg);
    pthread_exit(NULL);
}

void* dividir(void* arg) {
    printf("quociente: %d\n", a / b);

    int id = *((int*)arg);

    int rc = pthread_detach(nucleos[id]);
    if(rc){
        printf("Falha no detach da thread %d\n", id);
        exit(-1);
    }
    // liberando o nucleo
    pthread_mutex_lock(&mutex2);
    ocupado[id] = 0;
    pthread_mutex_unlock(&mutex2);


    free(arg);
    pthread_exit(NULL);
}

void* agenda(void* arg) {

    while (1) {
        // gerando numero aleatorio para escolher a funcao
        int num = rand() % NUM_FUNC;
        // criando a funcao
        Funcao func;
        // copiando o nome da funcao
        strcpy(func.nome, nomes_funcao[num]);
        // escolhendo a funcao
        switch (num) {
        case 0:
            func.ptr = somar;
            break;
        case 1:
            func.ptr = subtrair;
            break;
        case 2:
            func.ptr = multiplicar;
            break;
        default:
            func.ptr = dividir;
            break;
        }

        pthread_mutex_lock(&mutex); // lock

        while (lista_pronto->size == BUFFER_SIZE) {
            // se a fila estiver cheia, dorme
            printf("FILA CHEIA!\n");
            pthread_cond_wait(&pode_encher, &mutex);
        }
        enqueue(lista_pronto, func);
        // acordar o consumidor caso a fila esteja enchendo
        if (lista_pronto->size == 1) pthread_cond_signal(&pode_esvaziar);

        pthread_mutex_unlock(&mutex);
    }
}

void* algoritmo(void* arg) {

    while (1) {

        pthread_mutex_lock(&mutex); // lock

        while (lista_pronto->size == 0) { // se a fila estiver vazia, dorme
            printf("FILA VAZIA!!!\n");
            pthread_cond_wait(&pode_esvaziar, &mutex);
        }
        int* id[N];
        // percorre os nucleos
        for (int i = 0; i < N && lista_pronto->size != 0; i++) {
             id[i] = (int*)malloc(sizeof(int));
            *id[i] = i;
            // se o nucleo estiver livre, executa a funcao
        
            pthread_mutex_lock(&mutex2);
            if (ocupado[i] != 1) {
                Funcao temp = lista_pronto->front->next->funcao;
                ocupado[i] = 1; // ocupando o nucleo
                int rc = pthread_create(&(nucleos[i]), NULL, temp.ptr, (void*)id[i]);
                if (rc) {
                    printf("falha na criacao das threads erro: %d\n", rc);
                    exit(-1);
                }
                dequeue(lista_pronto);
                // acordar o produtor caso a fila esteja esvaziando
                if (lista_pronto->size == BUFFER_SIZE - 1) pthread_cond_signal(&pode_encher);
            }
            else{
                free(id[i]);
            }
            pthread_mutex_unlock(&mutex2);
            
        }
        pthread_mutex_unlock(&mutex);

    }

    pthread_exit(NULL);
}

int main(void) {

    srand(time(NULL)); // seed para gerar numeros aleatorios
    lista_pronto = create_queue(); // criando fila de funcoes
    int rc;
    
    rc = pthread_create(&escalonador, NULL, algoritmo, NULL); // criando thread escalonador
    if(rc){
        printf("erro na criação da thread\n");
        exit(-1);
    }
    rc = pthread_create(&enfileirador, NULL, agenda, NULL); // criando thread enfileirador
    if(rc){
        printf("erro na criação da thread\n");
        exit(-1);
    }

    rc = pthread_join(escalonador, NULL); // esperando thread escalonador terminar
    if(rc){
        printf("erro na execucao da thread\n");
        exit(-1);
    }
    rc = pthread_join(enfileirador, NULL); // esperando thread enfileirador terminar
    if(rc){
        printf("erro na execucao da thread\n");
        exit(-1);
    }

    pthread_exit(NULL);
    return 0;
}
