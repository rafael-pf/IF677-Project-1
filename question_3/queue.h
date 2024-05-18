#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct queue Queue;
typedef struct requisicao Requisicao;

/*cria a fila de requisicoes a serem resolvidas pelo banco*/
Queue* create_queue();
/*insere uma requisicao no final da fila*/
void enqueue(Queue* q, Requisicao req);
/*retira a primeira requisicao da fila*/
void dequeue(Queue* q);
/*limpa toda a fila*/
void clear(Queue* q);

#endif