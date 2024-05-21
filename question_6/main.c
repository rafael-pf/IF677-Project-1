#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "queue.h"

#define NUM_THREADS 4

void* somar(int a, int b) {
    printf("soma: %d\n", a + b);

    pthread_exit(NULL);
}

void* subtrair(int a, int b) {
    printf("subtracao: %d\n", a - b);

    pthread_exit(NULL);
}

void* multiplicar(int a, int b) {
    printf("produto: %d\n", a * b);

    pthread_exit(NULL);
}

void* dividir(int a, int b) {
    printf("quociente: %d\n", a / b);

    pthread_exit(NULL);
}

void carregaFuncoes(Funcao* array, int size) {

    strcpy(array[0].nome, "Somar");
    array[0].ptr = somar;
    strcpy(array[1].nome, "Subtrair");
    array[1].ptr = subtrair;
    strcpy(array[2].nome, "Multiplicar");
    array[2].ptr = multiplicar;
    strcpy(array[3].nome, "Dividir");
    array[3].ptr = dividir;

}

int main() {

    Queue* fila = create_queue();

    unsigned int size = 4;
    Funcao funcoes[size];

    carregaFuncoes(funcoes, size);

    return 0;
}