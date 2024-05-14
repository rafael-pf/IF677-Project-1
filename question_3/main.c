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
#define NUM_THREADS 4  // 3 clientes + banco

typedef struct {
    int id;
    int saldo;
} Conta; // cada conta possui indentificador e saldo

typedef struct {
    Conta conta;
    char* arquivo;
} InputAcesso; // sera passado como argumento para a funcao, contem a conta do usuario e o arquivo com as operações a serem realizadas por ele

char arquivos[NUM_THREADS - 1][20] = { {"cliente1.txt"}, {"cliente2.txt"}, {"cliente3.txt"} }; // nome dos arquivos (qtd de arquivos = qtd de acessos simultaneos)

// operações disponiveis no banco
int SACAR() {}
int DEPOSITAR() {}
int CONSULTAR() {}

// funcao das thread de clientes
void* ACESSO_AO_BANCO(void* acesso) {}

// funcao da thread do banco
void* BANCO() {}

int main() {
    pthread_t threads[NUM_THREADS];
    InputAcesso* acessos;

    acessos = malloc((NUM_THREADS - 1) * sizeof(InputAcesso));

    for (int i = 0; i < NUM_THREADS; i++) {
        if (!i) {

        }
        else {
            acessos[i].arquivo = arquivos[i - 1];
            acessos[i].conta.id = i - 1;
            acessos[i].conta.saldo = 0;

            int rc = pthread_create(&(threads[i]), NULL, ACESSO_AO_BANCO, (void*)&(acessos[i])); //criando threads
            if (rc) {
                printf("falha na criacao das threads\n");
                exit(-1);
            }
        }
    }

    return 0;
}


/*
    ideia: clientes fazem requisições para o banco,
    as requisições entram numa queue,
    banco usa queue para se guiar,
    duvida: como usar mutex para isso
*/