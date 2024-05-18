#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct {
    int idConta;
    char pedido[20];
    int valor;
} Requisicao;

typedef struct node {
    Requisicao requisicao;
    struct node* next;
} Node;

typedef struct {
    int size;
    Node* rear;
    Node* front;
}Queue;

/*Cria a fila de requisições a serem resolvidas pelo banco.*/
Queue* create_queue();
/*Insere uma requisição no final da fila.*/
void enqueue(Queue* q, Requisicao req);
/*Retira a primeira requisição da fila.*/
void dequeue(Queue* q);
/*Limpa toda a fila.*/
void clear(Queue* q);

#endif