#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

static Node* create_empty_node(Node* next) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->next = next;
    return n;
}

static Node* create_node(Funcao req, Node* next) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->funcao = req;
    n->next = next;
    return n;
}

Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = create_empty_node(NULL);
    q->size = 0;
    return q;
}

void enqueue(Queue* q, Funcao req) {
    q->rear->next = create_node(req, NULL);
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
    for (int i = 0; i < q->size; i++) {
        dequeue(q);
    }
    free(q);
}