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

int cont = 0;

int valida(int linha, int coluna, int posi, int posj){
    int erro = 0;
    for(int i = 0; i < NUM_THREADS; i++){
        if(i != posi && tabuleiro[posi][posj] == tabuleiro[i][posj]){
            erro++;
        }
    }

    for(int j = 0; j < NUM_THREADS; j++){
        if(j != posj && tabuleiro[posi][posj] == tabuleiro[posi][j]){
            erro++;
        }
    }

    for(int i = (linha - 1) * 3; i < 3 * linha; i++){
        for(int j = (coluna - 1) * 3; j < 3 * coluna; j++){
            if(i != posi && j != posj && tabuleiro[posi][posj] == tabuleiro[i][j]){
                erro++;
            }
        }
    }
    // printf("%d\n", erro);
    if(erro > 0){
        return 0;
    }
    else{
        return 1;
    }

    return 0;
}

void *checa(void *arg){
    int indice = *((int *) arg);
    int linha, coluna = (indice % 3) + 1;

    if(indice <= 3){
        linha = 1;
    }
    else if(indice > 3  && indice <= 6){
        linha = 2;
    }
    else{
        linha = 3;
    }
    
    //printf("LINHA %d DE QUADRADOS:\n", linha);
    for(int i = (linha - 1) * 3; i < 3 * linha; i++){
        for(int j = (coluna - 1) * 3; j < 3 * coluna; j++){
            pthread_mutex_lock(&mutex);
            cont += valida(linha, coluna , i, j);
            pthread_mutex_unlock(&mutex);
        }
    }
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

    if(cont == 81){
        printf("O sudoku é válido\n");
    }
    else{
        printf("O sudoku é inválido\n");
    }

    pthread_exit(NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}