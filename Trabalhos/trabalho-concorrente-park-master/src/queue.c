#include <stdio.h>
#include <stdlib.h>
#include <queue.h>

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

// Cria uma nova fila
Queue *create_queue() {
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  queue->front = NULL;
  queue->rear = NULL;
  return queue;
}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

// Verifica se a fila está vazia
int is_queue_empty(Queue *queue) {
  return (queue->front == NULL);
}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

// Enfileira um elemento
void enqueue(Queue *queue, int data) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data = data;
  newNode->next = NULL;

  if (is_queue_empty(queue)) {
    queue->front = queue->rear = newNode;
  } else {
    queue->rear->next = newNode;
    queue->rear = newNode;
  }

}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

// Desenfileira um elemento
int dequeue(Queue *queue) {
  if (is_queue_empty(queue)) {
    return -1;
  }

  int data = queue->front->data;
  Node *temp = queue->front;
  queue->front = queue->front->next;

  if (queue->front == NULL) {
    queue->rear = NULL;
  }

  free(temp);
  return data;
}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

// Mostra a fila
void print_queue(Queue *queue) {
  if (is_queue_empty(queue)) {
    return;
  }

  Node *temp = queue->front;
  while (temp != NULL) {
    printf("%d ", temp->data);
    temp = temp->next;
  }

  printf("\n");
}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

// Destroi a fila
void destroy_queue(Queue *queue) {
  while (!is_queue_empty(queue)) {
    dequeue(queue);
  }
  free(queue);
}
