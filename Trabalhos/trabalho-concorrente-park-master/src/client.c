/**
 * Esse arquivo tem como objetivo a implementação do cliente em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "client.h"
#include "queue.h"
#include "shared.h"
#include "toy.h"

// Declaração das assinaturas das funções privadas (não disponíveis em client.h)
void *enjoy(void *arg);
void buy_coins(client_t *self);
void wait_ticket(client_t *self);
void queue_enter(client_t *self);
toy_t* choose_toy(client_t* self);

pthread_t *threads_clientes; // Vetor de threads de clientes inicializadas no open_gate

// Thread que implementa o fluxo do cliente no parque.
void *enjoy(void *arg){
    client_t* client = (client_t*) arg; // converte o void* para client_t*
    wait_ticket(client); // cliente espera pela liberação da bilheteria

    // Ir brincar
    while (client->coins > 0) {
        toy_t* toy = choose_toy(client);
        client->coins--;
        debug("[INFO] - Cliente [%d] esperando para entrar no brinquedo [%d]!\n", client->id, toy->id);
        access_toy(toy, client->id);
    }

    debug("[EXIT] - O turista saiu do parque.\n");

    pthread_exit(NULL);
}

toy_t* choose_toy(client_t *self){
    return self->toys[(rand() % self->number_toys)];
}

// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){
    self->coins = (rand() % MAX_COINS) + 1;
}

// Função onde o cliente espera a liberacao da bilheteria para adentrar ao parque.
void wait_ticket(client_t *self){
    debug("[WAITING] - Cliente [%d] esperando pela abertura da bilheteria\n", self->id);
    // essa função bloqueia o cliente até que a condition "open_tickets" seja satisfeita
    pthread_mutex_lock(&tickets_mutex);  // Trava o mutex para garantir acesso exclusivo à condição
    while (!tickets_isopen) {  // Verifica o estado da condição para evitar Spurious Wakeups
        pthread_cond_wait(&open_tickets_cond, &tickets_mutex);  // Espera pela liberação da bilheteria
        // vale lembrar que o pthread_cond_wait quando chamado faz um unlock no tickets_mutex
        // e até que o sinal de condição válida (feito em ticket.c) seja feito, a thread do cliente fica bloqueada
    }
    pthread_mutex_unlock(&tickets_mutex); // Destrava o mutex relacionado à espera, significando que a bilheteria foi aberta

    queue_enter(self); // Cliente entra na fila da bilheteria
}

// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self) {
    pthread_mutex_lock(&queue_mutex); // Trava o mutex para garantir a entrada exclusiva de um cliente por vez na fila da bilheteria
    enqueue(gate_queue, self->id);   // Cliente entra na fila da bilheteria (id do cliente é colocado na fila indicando que o cliente entrou)
    sem_post(&queue_semaphore); // Dá um post no semáforo da fila, indicando que um cliente entrou
    pthread_mutex_unlock(&queue_mutex); // Destrava o mutex, permitindo que um próximo cliente entre na fila
    debug("[WAITING] - Turista [%d] entrou na fila do portao principal\n", self->id);

    sem_wait(&self->semaphore); // Bloqueia o cliente, para que ele só faça a compra depois de ser atendido (espera na fila enquanto não for atendido)

    buy_coins(self); // Após o cliente ser retirado da fila, ele vai realizar a compra de suas moedas, chamando buy_coins
    debug("[CASH] - Turista [%d] comprou [%d] moedas.\n", self->id, self->coins);
}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar os clientes.
void open_gate(client_args *args){
    array_clients = args->clients;
    n_clients = args->n; // n_clients recebe o número de threads recebida de args (da main)
    threads_clientes = (pthread_t*) malloc(sizeof(pthread_t)*n_clients); // aloca o vetor de n_clients
    for (int i = 0; i < n_clients; i++) {
        sem_init(&array_clients[i]->semaphore, 0, 1); // instancia o mutex do cliente 
        pthread_create(&threads_clientes[i], NULL, enjoy, (void *)args->clients[i]); // cria uma thread por cliente chamando o método enjoy
    }
}

// Essa função deve finalizar os clientes
void close_gate(){
    for (int i = 0; i < n_clients; i++) {
        pthread_join(threads_clientes[i], NULL); // finaliza a thread de cada cliente
        sem_destroy(&array_clients[i]->semaphore); // destroy o semaforo de cada cliente
    }
    pthread_mutex_lock(&close_park_mutex); // Garante a atomicidade da escrita da variável close_park
    close_park = 1; // Informa que o park foi fechado pois todos os clientes já saíram dele
    pthread_mutex_unlock(&close_park_mutex); // Desbloqueia o mutex
    for (int i = 0; i < n_toys; i++) 
        sem_post(&array_toys[i]->join_semaphore); // Faz um for desbloqueando todos os brinquedos que estão aguardando por clientes (mas não há mais nenhum); 
    free(threads_clientes); // desaloca o vetor de threads
}
