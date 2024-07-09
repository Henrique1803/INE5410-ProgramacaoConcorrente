#ifndef __DEFS_H__
#define __DEFS_H__

/* Essa biblioteca implementa definicoes que sao usadas pelo programa. */
/* ATENÇÃO: NÃO APAGUE OU EDITE O NOMES FORNECIDOS */

#include <pthread.h>
#include <semaphore.h>

#define TRUE 1              // Em C nao temos True ou False (como no Python). Usamos macro TRUE para True (como no Python). 
#define FALSE 0             // e FALSE para False (como no Python).


#define MAX_CAPACITY_TOY    10  // Capacidade maxima dos brinquedos.
#define MIN_CAPACITY_TOY    5   // Capacidade minima dos brinquedos.
#define EXECUTION_TIME_TOY  3   // Tempo de execução dos brinquedos.
#define WAIT_TIME_TOY       2   // Tempo de espera para entrada de clientes
#define MAX_COINS           20  // Maximo de moedas que um cliente pode comprar

#define DEBUG               1   //  Alterne (0 or 1) essa macro se voce espera desabilitar todas as mensagens de debug.



/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct ticket{
  pthread_t thread;         // A thread do funcionário que fica na bilheteria.
  int id;                   // O id do funcionario que fica na bilheteria.
} ticket_t;

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct toy{
  int id;                             // O id de um brinquedo.
  int capacity;                       // A capacidade total de um brinquedo.
  pthread_t thread;                   // A thread de um brinquedo.
  int clients_in_toy;                 // Número de clientes dentro do brinquedo.
  pthread_mutex_t n_clientes_mutex;   // Mutex responsável pelas operações com clients_in_toys.
  sem_t capacity_semaphore;           // Semáforo da capacidade de clientes que o brinquedo suporta.
  int enter_toy;                      // Variável de condição de entrada no brinquedo.
  pthread_mutex_t enter_mutex;        // Mutex responsável por enter_cond.
  pthread_cond_t enter_cond;          // Condição de entrada do brinquedo.
  sem_t join_semaphore;               // Semáforo de clientes que entraram no brinquedo.
  int exit_toy;                       // Variável de condição de saída.
  pthread_mutex_t exit_mutex;         // Mutex responsável por exit_cond.
  pthread_cond_t exit_cond;           // Condição de saída do brinquedo.
  sem_t ready_semaphore;              // Semáforo de controle de nova rodada do brinquedo.
} toy_t;

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct client{
  int id;                   // O id do cliente.
  int coins;                // Quantidade de moedas do cliente.
  int number_toys;          // Numero de brinquedos disponiveis.
  toy_t **toys;             // (Copy) Array de brinquedos.
  sem_t semaphore;          // Semaforo do cliente
} client_t;

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct{
  ticket_t **tickets;        // Array de funcionarios.                       
  int n;                    // Quantidade de funcionários da bilheteria.
} tickets_args;

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct{
  toy_t **toys;             // Array de brinquedos.
  int n;                    // Quantidade de brinquedos.
} toy_args;

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct{
  client_t **clients;       // Array de clientes.
  int n;                    // Quantidade de clientes.
} client_args;


/**************************************************
 *                    ATENÇÃO                     *
 *   NÃO EDITAR ESSE ARQUIVO A PARTIR DESTE PONTO *
 *                    ATENÇÃO                     *
 *************************************************/

// Estrutura da fila
typedef struct node {
  int data;                 // Dado da fila.
  struct node *next;        // Objeto do proximo item da fila.
} Node;

// Fila
typedef struct queue {
  Node *front;              // Primeiro elemento da fila.
  Node *rear;               // Ultimo elemento da fila.
} Queue;

#if DEBUG
#define debug(...) printf(__VA_ARGS__);
#else
#define debug(...);
#endif


#endif