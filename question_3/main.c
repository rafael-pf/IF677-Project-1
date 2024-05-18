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
#include <string.h>
#include <pthread.h>
#include "queue.h"
#define NUM_THREADS 4  // 3 clientes + banco

typedef struct {
    int id;
    int saldo;
} Conta; // cada conta possui indentificador e saldo

typedef struct {
    int idConta;
    char* pedido;
    int valor;
} Requisicao; // informacoes da requisicao atual a ser resolvida pelo banco

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
void* ACESSO_AO_BANCO(void* acesso) {
    InputAcesso* inputAcesso = (InputAcesso*)acesso;

    // separando informacoes do acesso ao banco
    char* nomeArquivo = inputAcesso->arquivo;
    int idConta = inputAcesso->conta.id;
    int saldoConta = inputAcesso->conta.saldo;

    FILE* arq = NULL;

    // leitura do arquivo
    arq = fopen(nomeArquivo, "r");

    if (arq == NULL) {
        printf("Falha ao abrir o arquivo\n");
        exit(1);
    }

    // processamento dos comandos do arquivo
    char texto[50] = {};
    char* comando = NULL;
    int qtd = 0;

    while (fscanf(arq, "%49[^,],\n", texto) == 1) {
        // processando cada linha do arquivo (<pedido> <valor?>,)
        comando = strtok(texto, " ");
        switch (comando[0]) {
        case 'C':
            printf("Cliente %d pediu pra consultar\n", idConta);
            break;
        case 'D':
            qtd = atoi(strtok(NULL, "\n"));
            printf("Cliente %d pediu pra depositar %d\n", idConta, qtd);
            break;
        case 'S':
            qtd = atoi(strtok(NULL, "\n"));
            printf("Cliente %d pediu pra sacar %d\n", idConta, qtd);
            break;
        default:
            break;
        }
    }

    fclose(arq);
}

// funcao da thread do banco
void* BANCO() {}

int main() {
    pthread_t threads[NUM_THREADS];
    InputAcesso* acessos;

    acessos = malloc((NUM_THREADS - 1) * sizeof(InputAcesso));

    for (int i = 0; i < NUM_THREADS; i++) {
        // primeira thread é a do banco, as outras sao dos clientes
        if (!i) {

        }
        else {
            acessos[i].arquivo = arquivos[i - 1];
            acessos[i].conta.id = i;
            acessos[i].conta.saldo = 0;

            int rc = pthread_create(&(threads[i]), NULL, ACESSO_AO_BANCO, (void*)&(acessos[i])); //criando threads
            if (rc) {
                printf("falha na criacao das threads\n");
                exit(-1);
            }
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (!i) {

        }
        else {
            pthread_join(threads[i], NULL);
        }
    }

    pthread_exit(NULL);
    free(acessos);

    return 0;
}

/*
    ideia: clientes fazem requisições para o banco,
    as requisições entram numa queue,
    banco usa queue para se guiar,
    duvida: como usar mutex para isso

    como usar mutex: variavel de condição,
    enquanto o banco estiver resolvendo uma req,
    trava as outras threads (leitura do arquivo
    e requisição ao banco)

*/
