/*
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "config.h"
#include "tickets.h"
#include "client.h"
#include "shared.h"
#include "queue.h"
#include "toy.h"

// Inicia a fila
void init_main_queue(){
    gate_queue = create_queue();
    pthread_mutex_init(&queue_mutex, NULL); // Inicializa o mutex para acesso a fila
    sem_init(&queue_semaphore, 0, 0); // Inicializa o semáforo da fila 
}

// Destroi a fila
void destroy_main_queue(){
    destroy_queue(gate_queue);
    pthread_mutex_destroy(&queue_mutex); // Destroy o semáforo da fila
    sem_destroy(&queue_semaphore); 
}

// Inicia a instância dos clientes
client_t **init_clients(int number, int toy_number, toy_t **toys){
    client_t **clients = malloc(number * sizeof(client_t *));
    for (int i = 0; i < number; i++){
        clients[i] = (client_t *) malloc(sizeof(client_t));
        clients[i]->id = i + 1;
        clients[i]->coins = 0;
        clients[i]->toys = toys;
        clients[i]->number_toys = toy_number;
    }
    return clients;
}

// Inicia a instância dos brinquedos
toy_t **init_toys(int number){
    toy_t **toys = malloc(number * sizeof(toy_t));
    for (int i = 0; i < number; i++){
        toys[i] = (toy_t *) malloc(sizeof(toy_t));
        toys[i]->id = i + 1;
        toys[i]->capacity = rand() % (MAX_CAPACITY_TOY - 1) + MIN_CAPACITY_TOY;
    }
    return toys;
}

// Inicia a instância dos funcionarios
ticket_t ** init_tickets(int number){
    ticket_t **tickets = malloc(number * sizeof(ticket_t));
    for (int i = 0; i < number; i++){
        tickets[i] = (ticket_t *) malloc(sizeof(ticket_t));
        tickets[i]->id = i + 1;
    }
    return tickets;
}

 // Desaloca os clientes
void finish_clients(client_t **clients, int number_clients){
    for (int i = 0; i < number_clients; i++){
        free(clients[i]);
    }
    free(clients);
}

 // Desaloca os brinquedos
void finish_toys(toy_t **toys, int number_toys){
    for (int i = 0; i < number_toys; i++){
        free(toys[i]);
    }
    free(toys);
}

 // Desaloca os funcionarios da bilheteria
void finish_tickets(ticket_t **tickets, int number_clients){
    for (int i = 0; i < number_clients; i++){
        free(tickets[i]);
    }
    free(tickets);
}

/**************************************************
 *                    ATENÇÃO                     *
 *   NÃO EDITAR ESSE ARQUIVO A PARTIR DESTE PONTO *
 *                    ATENÇÃO                     *
 *************************************************/

int main(int argc, char *argv[]){

    client_args *cli_args = (client_args *) malloc(sizeof(client_args));
    tickets_args *ticket_args = (tickets_args *) malloc(sizeof(tickets_args));
    toy_args *toys_args = (toy_args *) malloc(sizeof(toy_args));

    config_t _config = parse(argc, argv); // ./program -p (pessoas) -t (brinquedos) -g (bilheterias) -s (semente) -h (ajuda)
    srand(_config.seed); //Alimentando o gerador pseudo-aleatorio.

    debug("[STARTED] - O parque abriu suas portas.\n");

    //Iniciando a fila.
    init_main_queue();
    
    // Inicializa os brinquedos.
    toy_t **toys = init_toys(_config.toys);

    // Inicializa os clientes.
    client_t **clients = init_clients(_config.clients, _config.toys, toys);
    cli_args->clients = clients;
    cli_args->n = _config.clients;

    // Inicializa os funcionarios da bilheteria.
    ticket_t **tickets = init_tickets(_config.tickets);
    ticket_args->tickets = tickets;
    ticket_args->n = _config.tickets;
    
    // Recebe os argumentos para os brinquedos.
    toys_args->toys = toys;
    toys_args->n = _config.toys;

    // Ligando os brinquedos.
    open_toys(toys_args);
    // Os turistas entram no parque.
    open_gate(cli_args);
    // A bilheteria abre.
    open_tickets(ticket_args);

    // Os turistas saem do parque.
    close_gate();
    
    // A bilheteria fecha.
    close_tickets();

    // Desligam os brinquedos.
    close_toys();

    // Desalocando funcionarios
    finish_tickets(tickets, _config.tickets);

    // Desalocando clientes.
    finish_clients(clients, _config.clients);
    
    // Desalocando brinquedos.
    finish_toys(toys, _config.toys);

    /********************************************************************************
    *                                       EXCEÇÃO                                 *
    *           SINCRONIZE O FINAL DA EXECUÇÃO DO PROGRAMA PARTIR DESTE PONTO       *
    *                                       EXCEÇÃO                                 *
    *********************************************************************************/
    
    // Sincronize aqui
    pthread_mutex_destroy(&close_park_mutex);

    // Desalocando argumentos.
    free(cli_args);
    free(ticket_args);
    free(toys_args);

    // Destruindo filas
    destroy_main_queue();

    debug("[BYE] - O parque fechou suas portas.\n");

    return 0; /* "Houston, Tranquility Base here. The Eagle has landed." - Buzz Aldrin (July, 1969) */
}