#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 5

/*
Nesse codigo a quantidade de partes é controlada pelo NUM_THREADS.
O tamanho do vetor é lido do teclado e em sequencia é lido os números pelo teclado também.

O codigo divide o vetor em partes e essas partes são ordenadas por threads.
Uma barreira espera as threads terminarem a execução, para mandar uma outra thread juntar as partes e ordenar tudo.
*/

pthread_barrier_t barrier;

typedef struct conglomerado{
    int *array;
    unsigned int size;
}Conglomerado;

void swap(int *array, int i, int j){
    int tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
}

void *bubblesort(void *arg){//bubblesort tradicional
    Conglomerado *ptr = (Conglomerado *)arg;
    unsigned int size = ptr->size;
    int *array = ptr->array;
    for(int i=0;i<size-1;i++){
        for(int j=0;j<size-1-i;j++){
            if(array[j] > array[j+1]){
                swap(array, j, j+1);
            }
        }
    }
    pthread_barrier_wait(&barrier);
    pthread_exit(NULL);
}

void *merge(void *arg){//essa funcao vai pecorrer o vetor e inserindo os valores na posição correta
    Conglomerado *ptr = (Conglomerado *) arg;
    int *array = ptr->array;
    unsigned int size = ptr->size;
    for(int i=0;i<size;i++){
        int v = array[i];
        int j = i-1;
        while(j>=0 && array[j]>v){
            array[j+1] = array[j];
            j = j-1;
        }
        array[j+1] = v;
    }
    pthread_exit(NULL);
}

int main() {
    
    int size;
    scanf("%d", &size);
    int *vet = (int *) malloc(sizeof(int)*size);
    for(int i=0;i<size;i++){
        scanf("%d", &vet[i]);
    }

    pthread_t threads[NUM_THREADS];
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    unsigned int passo = size/NUM_THREADS;//Calculo quanto vai ser o tamanho das partes que vai dividir o vetor
    for(int i=0, thread_index=0; i<size && thread_index<NUM_THREADS; i+= passo, thread_index++){// i vai ser o quanto cada parte está deslocada da origem do vetor por isso ele cresce pelo tamanho de passo
        printf("Criando Thread %d\n", thread_index);//thread_index cresce no for também
        int rc = pthread_create(&threads[thread_index], NULL, bubblesort, ((void *) &(Conglomerado){.array=(vet+i), .size=passo}));//Passo para a thread o ponteiro para o vetor somado com o deslocamento e um inteiro com o tamanho do vetor
        if(rc){
            fprintf(stderr, "Erro na criacao do thread\n");
            exit(-1);
        }
    }
    
    for(int i=0; i<NUM_THREADS;i++){
        int rc = pthread_join(threads[i], NULL);
        if(rc){
            fprintf(stderr, "Erro na execução das threads\n");
            exit(-1);
        }
    }
    printf("\nImprimindo o vetor ordenado em partes de %d\n", passo);
    for(int i=0;i<size;i++){
        printf("%d ", vet[i]);//print para mostrar o vetor ordenado em partes
    }
    printf("\n\n");

    pthread_t thread_merge;
    int rc = pthread_create(&thread_merge, NULL, merge, ((void *) &(Conglomerado){.array=vet, .size=size}));//o vetor inteiro é passado para a função que vai juntar todas as partes
    if(rc){
        fprintf(stderr, "Erro na criacao do thread\n");
        exit(-1);
    }
    rc = pthread_join(thread_merge, NULL);
    if(rc){
        fprintf(stderr, "Erro na execução da thread\n");
        exit(-1);
    }

    printf("Imprimindo o vetor ordenado\n");
    for(int i=0;i<size;i++){
        printf("%d ", vet[i]);
    }
    printf("\n");



    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);
    free(vet);
    return 0;
}
