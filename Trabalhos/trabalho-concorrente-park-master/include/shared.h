#ifndef __SHARED_H__
#define __SHARED_H__

#include <defs.h>

// Você pode declarar novas funções (ou variaveis compartilhadas) aqui
extern client_t** array_clients;  // Array de clientes
extern int n_clients; // Número de clientes

extern toy_t** array_toys;  // Array de brinquedos
extern int n_toys; // Número de brinquedos

extern pthread_mutex_t tickets_mutex; // Mutex usado para controlar a abertura da bilheteria (clientes aguardam que esta seja aberta)
extern pthread_cond_t open_tickets_cond; // Condição que verifica se a bilheteria já foi aberta
extern int tickets_isopen; // variável que identifica se a bilheteria está aberta ou não

extern pthread_mutex_t queue_mutex; // Mutex usado para controlar a entrada dos clientes na fila da bilheteria
extern sem_t queue_semaphore; // Semáforo usado para controlar a entrada e saída dos clientes na fila da bilheteria

extern pthread_mutex_t close_park_mutex; // Mutex usado para encerrar o park
extern int close_park; // Variável usada para encerrar o park


/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
extern Queue *gate_queue;

#endif