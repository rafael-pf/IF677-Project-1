#include <stdio.h>
#include <stdlib.h>

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

Node* create_empty_node(Node* next) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (n == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    n->next = next;
    return n;
}

Node* create_node(Requisicao req, Node* next) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (n == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    n->requisicao = req;
    n->next = next;
    return n;
}

Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    Node* new_node = create_empty_node(NULL);
    q->rear = new_node;
    q->front = new_node;
    q->size = 0;
    return q;
}

void enqueue(Queue* q, Requisicao req) {
    if (q == NULL) {
        printf("Fila nao incializada!\n");
        exit(1);
    }
    Node* new_node = create_node(req, NULL);
    q->rear->next = new_node;
    q->rear = q->rear->next;
    q->size++;
}

void dequeue(Queue* q) {
    if (q->size != 0) {
        Node* tmp = q->front->next;
        q->front->next = q->front->next->next;
        if (q->front->next == NULL) {
            q->rear = q->front;
        }
        free(tmp);
        q->size--;
    }
}

void clear(Queue* q) {
    while (q->size)
    {
        dequeue(q);
    }

    free(q);
}