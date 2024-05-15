#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 5

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
            //printf("%d > %d?\n", j, j+1);
            if(array[j] > array[j+1]){
                swap(array, j, j+1);
            }
        }
    }
    pthread_barrier_wait(&barrier);
}

void *merge(void *arg){
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

    unsigned int start=size, passo = size/NUM_THREADS;
    for(int i=0, thread_index=0; i<size && thread_index<NUM_THREADS; i+= passo, thread_index++){
        printf("Criando Thread %d, passo:%d i:%d\n", thread_index, passo, i);
        pthread_create(&threads[thread_index], NULL, bubblesort, ((void *) &(Conglomerado){.array=(vet+i), .size=passo}));
        
    }
    
    for(int i=0; i<NUM_THREADS;i++){
        int rc = pthread_join(threads[i], NULL);
        if(rc){
            fprintf(stderr, "Erro na criacao do thread\n");
            exit(-1);
        }
    }

    pthread_t thread_merge;
    pthread_create(&thread_merge, NULL, merge, ((void *) &(Conglomerado){.array=vet, .size=size}));
    int rc = pthread_join(thread_merge, NULL);
    if(rc){
            fprintf(stderr, "Erro na criacao do thread\n");
            exit(-1);
        }

    for(int i=0;i<size;i++){
        printf("%d ", vet[i]);
    }
    printf("\n");



    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);
    free(vet);
    return 0;
}