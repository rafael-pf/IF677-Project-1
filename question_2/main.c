#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 3

pthread_barrier_t barrier;

typedef struct conglomerado{
    int *array;
    unsigned int start;
    unsigned int size;
}Conglomerado;

void swap(int *array, int i, int j){
    int tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
}

void *bubblesort(void *agregados){//bubblesort tradicional
    Conglomerado *ptr = (Conglomerado *)agregados;
    unsigned int start = ptr->start, size = ptr->size;
    int *array = ptr->array;
    for(int i=start;i<size-1;i++){
        for(int j=start;j<size-1-i;j++){
            //printf("%d > %d?\n", j, j+1);
            if(array[j] > array[j+1]){
                swap(array, j, j+1);
            }
        }
    }
    pthread_barrier_wait(&barrier);
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
    for(int i=0, j=0;i<size && j<NUM_THREADS;i+= passo, j++){
        printf("Criando Thread %d, size:%d start:%d\n", j, size, start);
        pthread_create(&threads[j], NULL, bubblesort, ((void *) &(Conglomerado){.array=vet, .size=size+start, .start=start}));
        start -= passo;
    }
    
    for(int i=0; i<NUM_THREADS;i++){
        int rc = pthread_join(threads[i], NULL);
        if(rc){
            fprintf(stderr, "Erro na criacao do thread\n");
            exit(-1);
        }
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