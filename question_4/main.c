/*
5 3 4 | 6 7 8 | 9 1 2
6 7 2 | 1 9 5 | 3 4 8
1 9 8 | 3 4 2 | 5 6 7
---------------------
8 5 9 | 7 6 1 | 4 2 3
4 2 6 | 8 5 3 | 7 9 1
7 1 3 | 9 2 4 | 8 5 6
---------------------
9 6 1 | 5 3 7 | 2 8 4
2 8 7 | 4 1 9 | 6 3 5
3 4 5 | 2 8 6 | 1 7 9
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 9 

int tabuleiro[NUM_THREADS][NUM_THREADS];
pthread_mutex_t mutex;

void *checa(void *arg){
    int indice = *((int *) arg);
    
    pthread_mutex_lock(&mutex);
    for(int i = (indice - 1) * 3; i < 3 * indice; i++){
        for(int j = (indice - 1) * 3; j < 3 * indice; j++){
            printf("%d ", tabuleiro[i][j]);
        }
        printf("\n");
    }
    pthread_mutex_unlock(&mutex);
}

int main(void){
    
    int *thread_id[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < NUM_THREADS; i++){
        for(int j = 0; j < NUM_THREADS; j++){
            scanf("%d", &tabuleiro[i][j]);
        }
    }

    // 1 pra cada quadrado
    pthread_t threads[NUM_THREADS];
    for(int i = 0; i < NUM_THREADS; i++){
        thread_id[i] = (int *) malloc(sizeof(int));
        *(thread_id[i]) = i + 1;
        int rc = pthread_create(&(threads[i]), NULL, checa, (void *) thread_id[i]);
        if(rc){
            printf("erro na criacao de threads\n");
            exit(-1);
        }
    }

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }


    pthread_exit(NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}