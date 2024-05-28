#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "queue.h"

#define NUM_THREADS 4
#define NUM_OPERATIONS 100

int a = 3;
int b = 2;

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
int nucleos_trabalhando = 0;

void segura() {
    for (int i = 0; i < 100000; i++) {}
    printf("nucleos trabalhando: %d\n", nucleos_trabalhando);
    if (nucleos_trabalhando > 4) printf("\tDeu merda\n");
}

void* somar(void* arg) {
    printf("soma: %d\n", a + b);

    segura();

    pthread_mutex_lock(&mymutex);
    nucleos_trabalhando--;
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mymutex);

    pthread_exit(NULL);
}

void* subtrair(void* arg) {
    printf("subtracao: %d\n", a - b);

    segura();

    pthread_mutex_lock(&mymutex);
    nucleos_trabalhando--;
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mymutex);

    pthread_exit(NULL);
}

void* multiplicar(void* arg) {
    printf("produto: %d\n", a * b);

    segura();

    pthread_mutex_lock(&mymutex);
    nucleos_trabalhando--;
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mymutex);

    pthread_exit(NULL);
}

void* dividir(void* arg) {
    printf("quociente: %d\n", a / b);

    segura();

    pthread_mutex_lock(&mymutex);
    nucleos_trabalhando--;
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mymutex);

    pthread_exit(NULL);
}

void carregaFuncoes(Funcao* array, int size) {

    strcpy(array[0].nome, "Somar");
    array[0].ptr = somar;
    strcpy(array[1].nome, "Subtrair");
    array[1].ptr = subtrair;
    strcpy(array[2].nome, "Multiplicar");
    array[2].ptr = multiplicar;
    strcpy(array[3].nome, "Dividir");
    array[3].ptr = dividir;

}

void carregaFila(Funcao* array, int size, Queue* queue) {
    srand(time(NULL));
    int index = rand() % size;

    for (int i = 0; i < NUM_OPERATIONS; i++) {
        pthread_mutex_lock(&mymutex);

        enqueue(queue, array[index]);
        pthread_cond_signal(&full);

        pthread_mutex_unlock(&mymutex);

        for (int j = 0; j < 100000; j++);

        index = rand() % size;
    }
}

void* escalonador(void* arg) {

    Queue* lista_pronto = (Queue*)arg;
    int size = lista_pronto->size;
    pthread_t threads;

    while (1) {
        pthread_mutex_lock(&mymutex);
        Funcao f = lista_pronto->front->next->funcao;
        dequeue(lista_pronto);

        // cria thread
        int rc = pthread_create(&(threads), NULL, f.ptr, NULL);
        if (rc) {
            printf("erro na execucao de threads\n");
            exit(-1);
        }

        nucleos_trabalhando++;
        while (!lista_pronto->size) {
            printf("escalonador dormindo, fila vazia.\n");
            pthread_cond_wait(&full, &mymutex);
        }
        while (nucleos_trabalhando == NUM_THREADS) {
            printf("escalonador dormindo, todos os nucleos ocupados.\n");
            pthread_cond_wait(&full, &mymutex);
        }
        pthread_mutex_unlock(&mymutex);
    }



    pthread_exit(NULL);
}

int main() {

    Queue* queue = create_queue();
    pthread_t thread_escalonador;
    unsigned int size = 4;
    Funcao funcoes[size];

    carregaFuncoes(funcoes, size);

    int rc = pthread_create(&thread_escalonador, NULL, escalonador, (void*)queue);
    if (rc) {
        printf("erro na execucao de threads\n");
        exit(-1);
    }

    carregaFila(funcoes, size, queue);

    pthread_join(thread_escalonador, NULL);

    pthread_mutex_destroy(&mymutex);
    pthread_cond_destroy(&full);
    pthread_exit(NULL);

    return 0;
}