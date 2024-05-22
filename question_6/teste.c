#include <stdio.h>

typedef void* (*FuncaPtr)(int, int);

void* somar(int a, int b) {
    printf("soma: %d\n", a + b);
}

void* subtrair(int a, int b) {
    printf("subtracao: %d\n", a - b);
}

void* multiplicar(int a, int b) {
    printf("produto: %d\n", a * b);
}

void* dividir(int a, int b) {
    printf("quociente: %d\n", a / b);
}

typedef struct {
    FuncaPtr ptr;
    char nome[30];
} Funcao;

int main() {

    //fato curioso ai se vc faz uma tabela tipo hashtable com ponteiros para funcao vc ta literalmente quase criando oop em c

    Funcao f[4];

    f[0].ptr = somar;
    f[1].ptr = subtrair;
    f[2].ptr = multiplicar;
    f[3].ptr = dividir;

    for (int i = 0; i < 4; i++) {
        f[i].ptr(3, 2);
    }


    return 0;
}