/**
 * Esse arquivo tem como objetivo a implementação da bilheteria em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS.
*/
#include <stdio.h>
#include <stdlib.h>
#include <tickets.h>
#include <unistd.h>

#include <queue.h>
#include "shared.h"

pthread_t *threads_tickets; // Vetor de threads de atendentes inicializadas no open_tickets
int n_tickets; // Número de atendentes recebido como parâmetro da main
int counter_workers = 0; // Contador de atendentes inicializados (utilizado para determinar quando a bilheteria abriu - último funcionário)
pthread_mutex_t counter_workers_mutex; // Mutex para o contador de atendentes (variável global)
int counter_clients = 0; // Contador de clientes atendidos pelas worker threads
int has_clients = TRUE; // Variável compartilhada entre threads como condição se ainda há clientes a serem atendidos
pthread_mutex_t counter_clients_mutex; // Mutex que controla a exclusão mútua do counter_clients

// Thread que implementa uma bilheteria
void *sell(void *args){
    pthread_mutex_lock(&counter_workers_mutex); // Mutex que bloqueia a região crítica do counter (e também neste caso da condition tickets_isopen)
    counter_workers++; // contador é incrementado, assim conseguimos saber quantos atendentes foram inicializados
    if (counter_workers == n_tickets) { // condição para abrir a bilheteria (apenas último atendente)
        debug("[INFO] - Bilheteria Abriu!\n"); // printa que a bilheteria abriu (apenas uma vez)
        tickets_isopen = 1; // a variável global que identifica se a bilheteria está aberta é alterada para 1 (true)
        pthread_cond_broadcast(&open_tickets_cond); // utilizamos o cond_boradcast para sinalizar a TODAS as threads dos clientes que a bilheteria está aberta
    }
    pthread_mutex_unlock(&counter_workers_mutex); // desbloqueia o mutex do contador de atendentes inicializados

    while (TRUE) { // Utiliza a ideia de bag of work para as threads funcionárias
        if(n_clients == 0){
            break;
        }
        sem_wait(&queue_semaphore); // Espera até que haja um cliente na fila

        pthread_mutex_lock(&queue_mutex); // Bloqueia o mutex, para que a retirada do cliente da fila sejá feita de forma atômica
        if (!has_clients) { // Verifica se ainda há clientes a serem atendidos
            pthread_mutex_unlock(&queue_mutex); // Desbloqueia o mutex da fila
            sem_post(&queue_semaphore); // Ponto chave do funcionamento: desbloqueia as threads que possivelmente estão esperando por clientes caso não haja (evita deadlock)
            break; // Sai do loop se não houver mais clientes
        }
        int id_cliente = dequeue(gate_queue); // Senão, retira o cliente da fila para que seja atendido
        pthread_mutex_unlock(&queue_mutex); // Desbloqueia o mutex do acesso a fila 
        
        sem_post(&array_clients[id_cliente-1]->semaphore); // Destrava o cliente que será atendido, ou seja, o cliente que foi retirado da fila, com base no seu id
        
        pthread_mutex_lock(&counter_clients_mutex); // trava o mutex do contador de clientes atendidos
        counter_clients++; // incrementa o número de clientes atendidos
        if (counter_clients >= n_clients) { // Verifica se todos os clientes já foram atendidos
            has_clients = FALSE; // Define has_clients como falso quando todos os clientes forem atendidos
            pthread_mutex_unlock(&counter_clients_mutex); // Desbloqueia o mutex do contador de clientes
            sem_post(&queue_semaphore); // Ponto chave do funcionamento: desbloqueia a as threads que possivelmente estão esperando por clientes caso não haja (evita deadlock)

            // Como apenas o funcionário que atender o último cliente irá executar esse código, ele printa que a bilheteria fechou
            debug("[INFO] - Bilheteria Fechou!\n") // Printa que a bilheteria fechou, após todos os atendentes retirarem os clientes da fila (ou seja, é possível que hajam prints de clientes realizando a compra de moedas mesmo após a bilheteria fechar! É o comportamento esperado segundo o README)
            tickets_isopen = 0; // Seta variável que controla o funcionamento da bilheteria para 0 (embora seja utilizado apenas na inicialização)

            break; // Sai do loop (não é necessário atender mais clientes)
        }
        pthread_mutex_unlock(&counter_clients_mutex); // desbloqueia o mutex do contador de clientes
    }
    pthread_exit(NULL); // Encerra as threads dos atendentes
}

// Essa função recebe como argumento informações sobre a bilheteria e deve iniciar os atendentes.
void open_tickets(tickets_args *args){
    n_tickets = args->n; // n_tickets recebe o número de threads recebida de args (da main)
    threads_tickets = (pthread_t*) malloc(sizeof(pthread_t)*n_tickets); // aloca o vetor de n_tickets
    pthread_mutex_init(&counter_workers_mutex, NULL); // inicializa o mutex relacionado ao contador de funcionários inicializados
    pthread_mutex_init(&counter_clients_mutex, NULL); // inicializa o mutex relacionado ao contador de clientes atendidos
    pthread_mutex_init(&tickets_mutex, NULL); // inicializa o mutex relacionado à abertura da bilheteria
    for (int i = 0; i < n_tickets; i++) {
        threads_tickets[i] = args->tickets[i]->thread; // associa cada elemento do vetor de threads com a thread já definida em tickets_t
        pthread_create(&threads_tickets[i], NULL, sell, (void *)args->tickets[i]); // cria uma thread por atendente chamando o método sell
    }
}

// Essa função deve finalizar a bilheteria
void close_tickets(){
    for (int i = 0; i < n_tickets; i++)
        pthread_join(threads_tickets[i], NULL); // finaliza a thread de cada atendente

    pthread_mutex_destroy(&counter_workers_mutex); // Destroy o mutex contador de funcionários inicializados
    pthread_mutex_destroy(&counter_clients_mutex); // Destroy o mutex contador de clientes atendidos
    pthread_mutex_destroy(&tickets_mutex); // Destroy o mutex de controle de abertura da bilheteria

    if(n_clients == 0) {
        debug("[INFO] - Bilheteria Fechou porque não há clientes na fila!\n"); //Caso em que há 0 clientes
    }

    free(threads_tickets); // desaloca o vetor de threads
}