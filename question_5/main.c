#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 2
#define LIN 2
#define COL 2

pthread_barrier_t barreira;
pthread_mutex_t mutex;

int coef[LIN][COL] = {{2,1}, {5, 7}};
int res[LIN] = {11, 13};
float var[LIN] = {1, 1};

int p = 2;

void *cria(void *arg){
    int id = *((int *) arg);

    int x = 1, k = 0;
    
    while(k < p){
        int sum = 0;
        for(int j = 0; j < NUM_THREADS; j++){
            if(id != j){
                sum += coef[id][j] * var[j];
            }
        }
        pthread_mutex_lock(&mutex);
        printf("id:%d sum:%d\n", id,sum);
        var[id] = (1.0 / coef[id][id]) * (res[id] - sum);
        printf("%f\n", var[id]);
        pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barreira);
        k++;
    }

}

int main(void){
    
    pthread_barrier_init(&barreira, NULL, NUM_THREADS);
    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[NUM_THREADS];
    int *taskID[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++){
        taskID[i] = (int *) malloc(sizeof(int));
        *taskID[i] = i;
        int rc = pthread_create(&threads[i], NULL, cria, (void*) taskID[i]);
        if(rc){
            printf("erro na criacao de threads\n");
            exit(-1);
        }
    }

    for(int i = 0; i < NUM_THREADS; i++){
        int rc = pthread_join(threads[i], NULL);
        if(rc){
            printf("erro na execucao de threads\n");
            exit(-1);
        }
    }

    for(int i = 0; i < NUM_THREADS; i++){
        printf("x%d = %f\n", i + 1, var[i]);
    }

    pthread_exit(NULL);
    pthread_barrier_destroy(&barreira);
    pthread_mutex_destroy(&mutex);

    for(int i = 0; i < NUM_THREADS; i++){
        free(taskID[i]);
    }

    return 0;
}