#ifndef _QUEUE_H
#define _QUEUE_H

typedef void* (*Funcptr)(void*);

typedef struct {
    Funcptr ptr;
    char nome[30];
} Funcao;

typedef struct node {
    Funcao funcao;
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
void enqueue(Queue* q, Funcao req);
/*Retira a primeira requisição da fila.*/
void dequeue(Queue* q);
/*Limpa toda a fila.*/
void clear(Queue* q);

#endif