#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 1
#define LIN 2 //(numero de equaçoes)
#define COL 2 //(numero de variaveis)

pthread_barrier_t barreira;
pthread_mutex_t mutex;

int num = COL / NUM_THREADS; //quantidade de variáveis por thread

int coef[LIN][COL] = { {2, 1}, {5, 7} }; //matriz de coeficicientes do sistema
int res[COL] = { 11, 13 }; //vetor de resultados do sistema
float var[COL] = { 1, 1}; //vetor de variáveis

typedef struct{ //struct que controla a divisao variavel/thread
    int *vet;
    int size;
}Conglomerado;

int p = 2; //controla a aproximação do algoritmo

//função que calcula as variáveis
void *cria(void * arg){

    Conglomerado *ptr = (Conglomerado *) arg;
    int *vet = ptr->vet; //vet é um vetor com os indices que a thread vai ter q lidar
    int size = ptr->size;
    int k=0;
    while(k<p){
        for(int i=0;i<size;i++){
            int ind = vet[i];
            //calculo do somatório
            float sum = 0; 
            for(int j = 0; j < COL; j++){
                if(ind != j){
                    sum += coef[ind][j] * var[j];
                }
            }
            
            pthread_barrier_wait(&barreira); //esperando as threads sincronizarem para impedir que uma passe da outra no loop
            //lidando com a região crítica (variável global)
            pthread_mutex_lock(&mutex);
            var[ind] = (1.0 / coef[ind][ind]) * (res[ind] - sum);
            pthread_mutex_unlock(&mutex);
            
            //esperando as threads calcularem xi 
        }
        pthread_barrier_wait(&barreira);
        k++;

    }
    free(arg);
    pthread_exit(NULL);
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
    int *taskID[NUM_THREADS]; //matriz de indices

    int passo = num, lastIndex=0;

    for(int i = 0; i < NUM_THREADS; i++){
        if(i == 0 && COL != (passo * NUM_THREADS)){ //primeira thread lida com o excedente de variáveis (caso haja)
            int vetorMaior = (COL + passo) - (passo * NUM_THREADS);

            taskID[i] = (int *) malloc(sizeof(int) * vetorMaior);
            for(int j=0;j<vetorMaior;j++){
                taskID[i][j] = j; //atribuindo os indices a matriz
            }
            lastIndex = vetorMaior;

            Conglomerado *ptr = (Conglomerado *) malloc(sizeof(Conglomerado)); //struct para passar dados
            ptr->size = vetorMaior;
            ptr->vet = taskID[i];

            int rc = pthread_create(&(threads[i]), NULL, cria, ((void *) ptr));
            if(rc){
                printf("erro na criacao de threads\n");
                exit(-1);
            }
        }
        else{ //para as demais threads lidarem com quantidades iguais de threads
            taskID[i] = (int *) malloc(sizeof(int) * passo);

            for(int j=0;j<passo && lastIndex < COL; j+=1, lastIndex+=1){
                taskID[i][j] = lastIndex;
            }

            Conglomerado *ptr = (Conglomerado *) malloc(sizeof(Conglomerado));
            ptr->size = passo; //isso diferencia a primeira parte da segunda (quantidade designada a respectiva thread)
            ptr->vet = taskID[i];

            int rc = pthread_create(&(threads[i]), NULL, cria, ((void *) ptr));
            if(rc){
                printf("erro na criacao de threads\n");
                exit(-1);
            }
        }
    }

    //join...
    for(int i = 0; i < NUM_THREADS; i++){
        int rc = pthread_join(threads[i], NULL);
        if(rc){
            printf("erro na execucao de threads\n");
            exit(-1);
        }
    }

    //printando a solução do sistema
    for(int i = 0; i < COL; i++){
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
