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
#define _XOPEN_SOURCE 600 

typedef struct {
    int id;
    int saldo;
} Conta; // cada conta possui indentificador e saldo

typedef struct {
    Conta conta;
    char* arquivo;
} InputAcesso; // sera passado como argumento para a funcao, contem a conta do usuario e o arquivo com as operações a serem realizadas por ele

char arquivos[NUM_THREADS - 1][20] = { {"cliente1.txt"}, {"cliente2.txt"}, {"cliente3.txt"} }; // nome dos arquivos (qtd de arquivos = qtd de acessos simultaneos)
InputAcesso acessos[NUM_THREADS - 1];
Queue* filaRequisicoes; // fila de requisicoes
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex para impedir concorrencia na manipulação da fila
pthread_barrier_t barrier; // barreira para inicializar a atuação do banco 

// operações disponiveis no banco
int SACAR(int valor, Conta* conta) {
    if (conta->saldo - valor < 0) {
        return -1;
    }
    conta->saldo -= valor;
    return conta->saldo;
}

int DEPOSITAR(int valor, Conta* conta) {
    conta->saldo += valor;
    return conta->saldo;
}

int CONSULTAR(Conta conta) {
    return conta.saldo;
}

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

            pthread_mutex_lock(&mutex);
            // enqueue(filaRequisicoes, ((Requisicao){.idConta = (idConta), .pedido = "CONSULTAR", .valor = (-1)}));
            pthread_mutex_unlock(&mutex);

            break;
        case 'D':
            qtd = atoi(strtok(NULL, "\n"));
            printf("Cliente %d pediu pra depositar %d\n", idConta, qtd);

            pthread_mutex_lock(&mutex);

            Requisicao req;

            req.idConta = idConta;
            strcpy(req.pedido, "DEPOSITAR");
            req.valor = qtd;

            // enqueue(filaRequisicoes, req);

            pthread_mutex_unlock(&mutex);

            break;
        case 'S':
            qtd = atoi(strtok(NULL, "\n"));
            printf("Cliente %d pediu pra sacar %d\n", idConta, qtd);

            pthread_mutex_lock(&mutex);
            // enqueue(filaRequisicoes, ((Requisicao){.idConta = (idConta), .pedido = "SACAR", .valor = (qtd)}));
            pthread_mutex_unlock(&mutex);

            break;
        default:
            break;
        }
    }

    fclose(arq);
    pthread_barrier_wait(&barrier);
    pthread_exit(NULL);
}

// funcao da thread do banco
void* BANCO(void* arg) {
    pthread_barrier_wait(&barrier);

    while (filaRequisicoes->size > 0) {
        Requisicao req = filaRequisicoes->front->requisicao;

        switch (req.pedido[0])
        {
        case 'C':
            for (int i = 0; i < NUM_THREADS; i++) {
                if (acessos[i].conta.id == req.idConta) {
                    int resposta = CONSULTAR(acessos[i].conta);
                    printf("O saldo da conta %d eh: %d\n", req.idConta, resposta);
                }
            }
            break;

        case 'D':
            for (int i = 0; i < NUM_THREADS; i++) {
                if (acessos[i].conta.id == req.idConta) {
                    int resposta = DEPOSITAR(req.valor, &(acessos[i].conta));
                    printf("%d depositado com sucesso! O novo saldo da conta %d eh: %d\n", req.valor, req.idConta, resposta);
                }
            }
            break;

        case 'S':
            for (int i = 0; i < NUM_THREADS; i++) {
                if (acessos[i].conta.id == req.idConta) {
                    int resposta = SACAR(req.valor, &(acessos[i].conta));
                    if (resposta == -1) {
                        printf("Nao foi possível sacar %d da conta %d\n", req.valor, req.idConta);
                    }
                    else {
                        printf("%d sacados com sucesso! o novo saldo da conta %d eh %d\n", req.valor, req.idConta, resposta);
                    }
                }
            }
            break;

        default:
            break;
        }

        pthread_mutex_lock(&mutex);
        dequeue(filaRequisicoes);
        pthread_mutex_unlock(&mutex);

    }


    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    filaRequisicoes = create_queue();

    for (int i = 0; i < NUM_THREADS; i++) {
        // primeira thread é a do banco, as outras sao dos clientes
        if (!i) {
            // acessos[i].conta.id = -1;
            // int rc = pthread_create(&(threads[i]), NULL, BANCO, NULL);
            // if (rc) {
            //     printf("falha na criacao das threads\n");
            //     exit(-1);
            // }
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
    pthread_barrier_destroy(&barrier);

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
