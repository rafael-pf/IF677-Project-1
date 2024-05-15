#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 2
#define LIN 2 //(numero de equaçoes)
#define COL 2 //(numero de variaveis)

pthread_barrier_t barreira;
pthread_mutex_t mutex;

int num = COL / NUM_THREADS; //quantidade de variáveis por thread
int resto = COL / NUM_THREADS;

int coef[LIN][COL] = {{2,1}, {5, 7}}; //matriz de coeficicientes do sistema
int res[LIN] = {11, 13}; //vetor de resultados do sistema
float var[LIN] = {1, 1}; //vetor de variáveis

/*
    O sistema usado de exemplo seria:
    {2x1 + x2 = 11
    {5x1 + 7x2 = 13
*/

int p = 30; //controla a aproximação do algoritmo

//função que calcula as variáveis
void *cria(void *arg){ 

    int id = *((int *) arg); //pegando id
    for(int i = 0; i < num; i++){
        id += i * num;
        int k = 0; //k da questao
        
        while(k < p){

            //calculo do somatório
            float sum = 0; 
            for(int j = 0; j < NUM_THREADS; j++){
                if(id != j){
                    sum += coef[id][j] * var[j];
                }
                pthread_barrier_wait(&barreira); //esperando as threads sincronizarem para impedir que uma passe da outra no loop
            }

            //lidando com a região crítica (variável global)
            pthread_mutex_lock(&mutex);
            var[id] = (1.0 / coef[id][id]) * (res[id] - sum);
            pthread_mutex_unlock(&mutex);
            k++;
            
            //esperando as threads calcularem xi 
            pthread_barrier_wait(&barreira); // ÈH O BRUNAOO
        }
    }
}

int main(void){
    
    if(LIN < COL){
        printf("o sistema é indeterminado\n");
        exit(1);
    }
    //inicialização dos recursos
    pthread_barrier_init(&barreira, NULL, NUM_THREADS);
    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[NUM_THREADS];
    int *taskID[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++){
        taskID[i] = (int *) malloc(sizeof(int));
        *taskID[i] = i;
        int rc = pthread_create(&(threads[i]), NULL, cria, (void*) taskID[i]);
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

    //printando a solução do sistema
    for(int i = 0; i < NUM_THREADS; i++){
        printf("x%d = %f\n", i + 1, var[i]);
    }

    //destruindo os recursos
    for(int i = 0; i < NUM_THREADS; i++){
        free(taskID[i]);
    }
    pthread_exit(NULL);
    pthread_barrier_destroy(&barreira);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}