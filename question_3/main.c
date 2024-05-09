/*
    3. Implemente um simulador de operações bancárias usando threads.
    O programa deve ter N threads representando clientes e uma thread
    representando o banco. Os clientes devem realizar operações como
    depósito, saque e consulta de saldo. O programa deve garantir que
    as operações sejam atômicas em uma mesma conta para evitar condições
    de corrida.

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_TRHEADS 4  // 3 clientes + banco

typedef struct {
    int id;
    int saldo;
} Conta;

char arquivos[NUM_TRHEADS - 1][20] = { {"cliente1.txt"}, {"cliente2.txt"}, {"cliente3.txt"} };

int SACAR() {}
int DEPOSITAR() {}
int CONSULTAR() {}

void* ACESSO_AO_BANCO() {}

int main() {


    return 0;
}
