/*
* * * | * * * | * * *
* 1 * | * 2 * | * 3 *
* * * | * * * | * * *
---------------------
* * * | * * * | * * *
* 4 * | * 5 * | * 6 *
* * * | * * * | * * *
---------------------
* * * | * * * | * * *
* 7 * | * 8 * | * 9 *
* * * | * * * | * * *

A resolução foi feita pensando em atribuir o papel de cada thread checar se seu quadrado é válido.
Cada número no centro representa o id da thread que cuida desse quadrado.
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 9 

int tabuleiro[NUM_THREADS][NUM_THREADS]; //Criando o tabuleiro
pthread_mutex_t mutex;
pthread_barrier_t barreira;

int cont = 0; //contador que checa a quantidade de números que estão inseridos de forma correta

int valida(int linha, int coluna, int posi, int posj) {
    int erro = 0; //variável de controle pra checar quais condições são falsas

    //para todos os casos abaixo não é considerado o pŕoprio elemento
    for (int i = 0; i < NUM_THREADS; i++) {
        if (i != posi && tabuleiro[posi][posj] == tabuleiro[i][posj]) { //se houver elemento igual na mesma linha
            erro++;
        }
    }

    for (int j = 0; j < NUM_THREADS; j++) {
        if (j != posj && tabuleiro[posi][posj] == tabuleiro[posi][j]) { //se houver elemento igual na mesma coluna
            erro++;
        }
    }

    for (int i = (linha - 1) * 3; i < 3 * linha; i++) {
        for (int j = (coluna - 1) * 3; j < 3 * coluna; j++) {
            if (i != posi && j != posj && tabuleiro[posi][posj] == tabuleiro[i][j]) { //se houver elemento igual no mesmo quadrado
                erro++;
            }
        }
    }

    if (erro > 0) { //se erro não for zero, há pelo menos um motivo pra o número não estar lá
        return 0;
    }
    else {
        return 1;
    }
}

void* checa(void* arg) { //essa função percorre cada um dos números do quadrado da respectiva thread
    int indice = *((int*)arg);
    int linha, coluna = (indice % 3) + 1;

    if (indice <= 3) { //threads 1-3 cuidam da "primeira" linha de quadrados
        linha = 1;
    }
    else if (indice > 3 && indice <= 6) { //threads 4-6 cuidam da "segunda linha de quadrados"
        linha = 2;
    }
    else { //o resto das threads cuidam da última linha de quadrados
        linha = 3;
    }

    for (int i = (linha - 1) * 3; i < 3 * linha; i++) {
        for (int j = (coluna - 1) * 3; j < 3 * coluna; j++) {
            if (valida(linha, coluna, i, j) == 1) { //se o número checado estiver na posição certa...
                pthread_mutex_lock(&mutex); //travamos o mutex, pois as threads irão acessar a mesma zona crítica (global cont)
                cont++;
                pthread_mutex_unlock(&mutex); //destrava o mutex
            }
        }
    }
    pthread_barrier_wait(&barreira);
}

int main(void) {

    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barreira, NULL, NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) { //recebendo a entrada
        for (int j = 0; j < NUM_THREADS; j++) {
            scanf("%d", &tabuleiro[i][j]);
        }
    }

    pthread_t threads[NUM_THREADS]; //cada thread cuida de um quadrado 3x3 do sudokus
    int* thread_id[NUM_THREADS]; //vetor de id de threads pra ajudar na identificação dos quadrados

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_id[i] = (int*)malloc(sizeof(int)); //alocando o ponteiro de int pra facilitar a conversão
        *(thread_id[i]) = i + 1;

        int rc = pthread_create(&(threads[i]), NULL, checa, (void*)thread_id[i]); //criando threads
        if (rc) {
            printf("erro na criacao de threads\n");
            exit(-1);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) { //dando join
        pthread_join(threads[i], NULL);
    }

    if (cont == NUM_THREADS * NUM_THREADS) { //se a quantidade de números certos for igual ao total de números, é válidos
        printf("O sudoku é válido\n");
    }
    else {
        printf("O sudoku é inválido\n");
    }

    //destruindo mutex e dando exit
    pthread_exit(NULL);
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barreira);

    return 0;
}