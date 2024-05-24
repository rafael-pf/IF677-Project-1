/*

    3. Implemente um simulador de operações bancárias usando threads.
    O programa deve ter N threads representando clientes e uma thread
    representando o banco. Os clientes devem realizar operações como
    depósito, saque e consulta de saldo. O programa deve garantir que
    as operações sejam atômicas em uma mesma conta para evitar condições
    de corrida.

*/
#define _XOPEN_SOURCE 600 

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
    Conta conta;
    char arquivo[30];
} InputAcesso; // sera passado como argumento para a funcao, contem a conta do usuario e o arquivo com as operações a serem realizadas por ele

char arquivos[NUM_THREADS - 1][20] = { {"cliente1.txt"}, {"cliente2.txt"}, {"cliente3.txt"} }; // nome dos arquivos (qtd de arquivos = qtd de acessos simultaneos)
int ids[NUM_THREADS - 1] = { 1,2,3 }; // ids das contas (altere conforme quiser, apenas estando atento á qtd de contas)
InputAcesso acessos[NUM_THREADS - 1]; // conta + input de requisiçoes
Queue* filaRequisicoes; // fila de requisicoes a serem processadas pelo banco
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex para manipulação da fila
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
    char texto[50];
    char* comando;
    int qtd = 0;

    while (fscanf(arq, "%49[^,],\n", texto) == 1) {
        // processando cada linha do arquivo (<pedido> <valor?>,)

        comando = strtok(texto, " ");
        switch (comando[0]) {
        case 'C':
            printf("[CONTA %d] Solicitação de consulta\n", idConta);

            // trava o mutex e coloca a requisicao na fila
            pthread_mutex_lock(&mutex);
            enqueue(filaRequisicoes, ((Requisicao){.idConta = (idConta), .pedido = "CONSULTAR", .valor = (-1)}));
            pthread_mutex_unlock(&mutex);

            break;
        case 'D':
            qtd = atoi(strtok(NULL, "\n"));
            printf("[CONTA %d] Solicitação de depósito de %d reais\n", idConta, qtd);

            // trava o mutex e coloca a requisicao na fila
            pthread_mutex_lock(&mutex);
            enqueue(filaRequisicoes, ((Requisicao){.idConta = idConta, .pedido = "DEPOSITAR", .valor = qtd}));
            pthread_mutex_unlock(&mutex);

            break;
        case 'S':
            qtd = atoi(strtok(NULL, "\n"));
            printf("[CONTA %d] Solicitação de saque de %d reais\n", idConta, qtd);

            // trava o mutex e coloca a requisicao na fila
            pthread_mutex_lock(&mutex);
            enqueue(filaRequisicoes, ((Requisicao){.idConta = (idConta), .pedido = "SACAR", .valor = (qtd)}));
            pthread_mutex_unlock(&mutex);

            break;
        default:
            break;
        }
    }

    fclose(arq);
    pthread_barrier_wait(&barrier); //sincronizar as threads de acesso, para que thread banco inicie apenas quando elas terminarem
    pthread_exit(NULL);
}

// funcao da thread do banco
void* BANCO(void* arg) {
    pthread_barrier_wait(&barrier); // iniciar resposta às requisições apenas quando os acessos todas estiverem na fila 

    while (filaRequisicoes->size > 0) {
        // processo cada uma de acordo com seu nome (analisa-se primeiro char)
        Requisicao req = filaRequisicoes->front->next->requisicao;
        switch (req.pedido[0])
        {
        case 'C':

            for (int i = 0; i < NUM_THREADS - 1; i++) {
                if (acessos[i].conta.id == req.idConta) {
                    int resposta = CONSULTAR(acessos[i].conta);
                    printf("[BANCO] O saldo da conta %d eh: %d\n", req.idConta, resposta);
                }
            }
            break;

        case 'D':
            for (int i = 0; i < NUM_THREADS - 1; i++) {
                if (acessos[i].conta.id == req.idConta) {
                    int resposta = DEPOSITAR(req.valor, &(acessos[i].conta));
                    printf("[BANCO] %d depositado com sucesso! O novo saldo da conta %d eh: %d\n", req.valor, req.idConta, resposta);
                }
            }
            break;

        case 'S':
            for (int i = 0; i < NUM_THREADS - 1; i++) {
                if (acessos[i].conta.id == req.idConta) {
                    int resposta = SACAR(req.valor, &(acessos[i].conta));
                    if (resposta == -1) {
                        printf("[BANCO] Nao foi possível sacar %d da conta %d\n", req.valor, req.idConta);
                    }
                    else {
                        printf("[BANCO] %d sacados com sucesso! O novo saldo da conta %d eh %d\n", req.valor, req.idConta, resposta);
                    }
                }
            }
            break;

        default:
            break;
        }

        // depois de processar, locka o mutex e da dequeue na fila de requisiçoes
        pthread_mutex_lock(&mutex);
        dequeue(filaRequisicoes);
        pthread_mutex_unlock(&mutex);

    }


    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_barrier_init(&barrier, NULL, NUM_THREADS); // barreira para controlar inicio da thread banco
    filaRequisicoes = create_queue(); // inicializacao da fila de requisicoes

    for (int i = 0; i < NUM_THREADS; i++) {
        // primeira thread é a do banco, as outras sao dos clientes
        if (!i) {
            int rc = pthread_create(&(threads[i]), NULL, BANCO, NULL);
            if (rc) {
                printf("falha na criacao das threads\n");
                exit(-1);
            }
        }
        else {
            // inicializa contas
            strcpy(acessos[i - 1].arquivo, arquivos[i - 1]);
            acessos[i - 1].conta.id = ids[i - 1];
            acessos[i - 1].conta.saldo = 0;

            int rc = pthread_create(&(threads[i]), NULL, ACESSO_AO_BANCO, (void*)&(acessos[i - 1])); //criando threads
            if (rc) {
                printf("falha na criacao das threads\n");
                exit(-1);
            }
        }
    }

    // join em todas as threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_exit(NULL);
    pthread_barrier_destroy(&barrier);
    clear(filaRequisicoes);

    return 0;
}