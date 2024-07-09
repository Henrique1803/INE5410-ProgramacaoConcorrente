#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <defs.h>

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

Queue *create_queue();
int is_queue_empty(Queue *queue);
void enqueue(Queue *queue, int data);
int dequeue(Queue *queue);
void print_queue(Queue *queue);
void destroy_queue(Queue *queue);

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

#endif