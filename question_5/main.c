#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 1
#define LIN 3 //(numero de equaçoes)
#define COL 3 //(numero de variaveis)
#define p 30  //controla a aproximação do algoritmo

pthread_barrier_t barreira;
pthread_mutex_t mutex;

int num = COL / NUM_THREADS; //quantidade de variáveis por thread

int coef[LIN][COL] = { {-3, 1, 1}, {2, 5, 1}, {2, 3, 7} }; //matriz de coeficicientes do sistema
int res[COL] = { 2, 5, -17 }; //vetor de resultados do sistema
float var[p][COL]; //matriz de variáveis na 'p' interacao

typedef struct { //struct que controla os indices que cada thread eh responsavel
    int* vet;
    int size;
}Conglomerado;

//função que calcula as variáveis
void* cria(void* arg) {
    Conglomerado* ptr = (Conglomerado*)arg;//vet é um vetor com os indices que a thread vai ter q lidar
    int* vet = ptr->vet;
    int size = ptr->size;
    int k = 0;
    while (k < p) {//laço do k até p 
        for (int i = 0;i < size;i++) {//laço para cada threads pecorrer seus respectivos indices sem avançar o k 
            int id = vet[i];
            //calculo do somatório
            float sum = 0;
            for (int j = 0; j < COL; j++) {
                if (id != j) {
                    sum += coef[id][j] * var[k][j];
                }
            }

            //lidando com a região crítica (variável global)
            pthread_mutex_lock(&mutex);
            var[k + 1][id] = (1.0 / coef[id][id]) * (res[id] - sum);
            pthread_mutex_unlock(&mutex);
        }
        pthread_barrier_wait(&barreira); //esperando as threads sincronizarem para impedir que uma avançe o k antes das outras
        k++;

    }
    free(arg);
    pthread_exit(NULL);
}

int main(void) {

    if (LIN < COL) { //mais variaveis que equacoes
        printf("o sistema é indeterminado\n");
        exit(1);
    }
    //inicialização dos recursos
    pthread_barrier_init(&barreira, NULL, NUM_THREADS);
    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[NUM_THREADS];
    int* taskID[NUM_THREADS];

    for (int i = 0;i < COL;i++) { //setando xi(0) como 1
        var[0][i] = 1;
    }

    // utilizamos uma logica de divisao de tarefas semelhante a da questão 2
    int passo = num, lastIndex = 0;
    for (int i = 0;i < NUM_THREADS;i++) {
        if (i == 0 && COL != (passo * NUM_THREADS)) {//caso a divisão de variaveis para as threads seja desigual, a thread 0 recebe os indices restantes
            int vetorMaior = (COL + passo) - (passo * NUM_THREADS);//qtd de indices que a thread 0 vai receber
            taskID[i] = (int*)malloc(sizeof(int) * vetorMaior);
            for (int j = 0;j < vetorMaior;j++) {
                taskID[i][j] = j;
            }
            lastIndex = vetorMaior;//para que a contagem da divisão de variaveis continue igual para as outras threads
            Conglomerado* ptr = (Conglomerado*)malloc(sizeof(Conglomerado));
            ptr->size = vetorMaior;
            ptr->vet = taskID[i];
            int rc = pthread_create(&(threads[i]), NULL, cria, ((void*)ptr));
            if (rc) {
                printf("erro na criacao de threads\n");
                exit(-1);
            }
        }
        else {
            taskID[i] = (int*)malloc(sizeof(int) * passo);
            for (int j = 0;j < passo && lastIndex < COL; j += 1, lastIndex += 1) {// verifica se j não passa do tamanho de passo e verifica se o lastIndex nao passa da ultima coluna
                taskID[i][j] = lastIndex;
            }
            Conglomerado* ptr = (Conglomerado*)malloc(sizeof(Conglomerado));
            ptr->size = passo;
            ptr->vet = taskID[i];
            int rc = pthread_create(&(threads[i]), NULL, cria, ((void*)ptr));
            if (rc) {
                printf("erro na criacao de threads\n");
                exit(-1);
            }
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_join(threads[i], NULL);
        if (rc) {
            printf("erro na execucao de threads\n");
            exit(-1);
        }
    }

    //printando a solução do sistema
    for (int i = 0; i < COL; i++) {
        printf("x%d = %f\n", i + 1, var[p - 1][i]);
    }

    //destruindo os recursos
    for (int i = 0; i < NUM_THREADS; i++) {
        free(taskID[i]);
    }
    pthread_exit(NULL);
    pthread_barrier_destroy(&barreira);
    pthread_mutex_destroy(&mutex);

    return 0;
}
