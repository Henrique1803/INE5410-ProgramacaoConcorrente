/**
 * Esse arquivo tem como objetivo a implementação de um brinquedo em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "toy.h"
#include "shared.h"

pthread_t *threads_toys; // Vetor de threads de brinquedos inicializadas no open_toys


void access_toy(toy_t* self, int client_id){

    //   fila das próximas rodadas    |->   clientes a entrar    |->   brinquedo
    sem_wait(&self->capacity_semaphore); // Cliente aguarda uma nova vaga no brinquedo

    // Faz as threads dos clientes esperarem com que o brinquedo libere sua entrada
    pthread_mutex_lock(&self->enter_mutex);
    while(!self->enter_toy) { // Verifica o estado da condição para evitar Spurious Wakeups
        pthread_cond_wait(&self->enter_cond, &self->enter_mutex); // Brinquedo bloqueia a entrada enquanto estiver em execução
    }
    pthread_mutex_unlock(&self->enter_mutex);

    // Cliente entra no brinquedo
    debug("[INFO] - Cliente [%d] entrou no brinquedo [%d].\n",client_id, self->id);
    pthread_mutex_lock(&self->n_clientes_mutex); // Garante a atomicidade da operação de incremento e comparação da variável clients_in_toys
    self->clients_in_toy++; // Incrementa a quantidade de clientes dentro do brinquedo atualmente
    if(self->clients_in_toy == 1) // Verifica se é o primeiro cliente a entrar no brinquedo
        sem_post(&self->join_semaphore); // Libera o brinquedo pra começar a contagem regressiva pra iniciar sua execução
    pthread_mutex_unlock(&self->n_clientes_mutex); // Desbloqueia o mutex

    // Faz as threads dos clientes esperarem com que o brinquedo libere sua saída
    pthread_mutex_lock(&self->exit_mutex);
    while(!self->exit_toy) { // Verifica o estado da condição para evitar Spurious Wakeups
        pthread_cond_wait(&self->exit_cond, &self->exit_mutex); // Brinquedo bloqueia a saída enquanto estiver em execução
    }
    pthread_mutex_unlock(&self->exit_mutex);

    // Cliente sai do brinquedo
    debug("[INFO] - Cliente [%d] saiu do brinquedo [%d].\n",client_id, self->id);
    pthread_mutex_lock(&self->n_clientes_mutex); // Garante a atomicidade da operação de decremento e comparação da variável clients_in_toys
    self->clients_in_toy--; // Decrementa a quantidade de clientes dentro do brinquedo atualmente
    if(self->clients_in_toy == 0) // Verifica se é o último cliente a sair do brinquedo
        sem_post(&self->ready_semaphore); // Libera o brinquedo pra que possa iniciar sua nova rodada
    pthread_mutex_unlock(&self->n_clientes_mutex); // Desbloqueia o mutex

    sem_post(&self->capacity_semaphore); // Cliente libera a vaga ocupada no brinquedo

}

// Thread que o brinquedo vai usar durante toda a simulacao do sistema
void *turn_on(void *args){
    toy_t* toy = (toy_t*) args; // converte o void* para toy_t*

    debug("[INFO] - Brinquedo [%d] em funcionamento!\n", toy->id);
    while(TRUE) { // Brinquedo vai executar enquanto houver clientes no parque
        debug("[IN] - Brinquedo [%d] aguardando clientes entrarem!\n", toy->id);
        toy->enter_toy = 1; // Entrada do brinquedo está liberada
        pthread_cond_broadcast(&toy->enter_cond); // Sinaliza a todos os clientes que estão aguardando a entrada
        sem_wait(&toy->join_semaphore); // Aguarda pelo menos um brinquedo entrar no brinquedo
        pthread_mutex_lock(&close_park_mutex); // Garante o acesso atômico na verificação da variável close_park
        if (close_park) {
            pthread_mutex_unlock(&close_park_mutex); // Libera o mutex
            break; // Encerra o while se não há mais nenhum cliente no park
        }
        pthread_mutex_unlock(&close_park_mutex); // Libera o mutex
        sleep(WAIT_TIME_TOY); // Aguarda um tempo de espera para que novos clientes entrem no brinquedo
        toy->enter_toy = 0; // Entrada está bloqueada, pois o brinquedo será ligado

        debug("[ON] - O brinquedo  [%d] foi ligado.\n", toy->id);
        sleep(EXECUTION_TIME_TOY); // Aguarda o tempo de execução do brinquedo
        debug("[OFF] - O brinquedo [%d] foi desligado.\n", toy->id);

        debug("[OUT] - Brinquedo [%d] aguardando clientes sairem!\n", toy->id);
        toy->exit_toy = 1; // Saída do brinquedo está liberada
        pthread_cond_broadcast(&toy->exit_cond); // Sinaliza a todos os clientes que estão aguardando a saída

        sem_wait(&toy->ready_semaphore); // Espera todos os clientes saírem para iniciar uma nova rodada
        toy->exit_toy = 0; // Saída está bloqueada 
    }
    debug("[INFO] - Brinquedo [%d] encerrado!\n", toy->id);

    pthread_exit(NULL);
}


// Essa função recebe como argumento informações e deve iniciar os brinquedos.
void open_toys(toy_args *args){
    n_toys = args->n; // n_toys recebe o número de threads recebida de args (da main)
    array_toys = args->toys;
    threads_toys = (pthread_t*) malloc(sizeof(pthread_t)*n_toys); // aloca o vetor de n_toys
    for (int i = 0; i < n_toys; i++) {
        threads_toys[i] = args->toys[i]->thread; // associa cada elemento do vetor de threads com a thread já definida em toys_t
        args->toys[i]->clients_in_toy = 0; // quantidade de clientes dentro do brinquedo inicia em zero
        pthread_mutex_init(&args->toys[i]->n_clientes_mutex, NULL); // inicializa o mutex responsável pelas operações com clients_in_toys
        sem_init(&args->toys[i]->capacity_semaphore, 0, args->toys[i]->capacity); // inicializa o semáforo da capacidade de clientes que o brinquedo suporta
        args->toys[i]->enter_toy = 0; // entrada do brinquedo começa bloqueada
        pthread_mutex_init(&args->toys[i]->enter_mutex, NULL); // inicializa o mutex responsável por enter_cond
        pthread_cond_init(&args->toys[i]->enter_cond, NULL); // inicializa a condição de entrada do brinquedo
        sem_init(&args->toys[i]->join_semaphore, 0, 0); // inicializa o semáforo de clientes que entraram no brinquedo em zero
        args->toys[i]->exit_toy = 0; // saída do brinquedo começa bloqueada
        pthread_mutex_init(&args->toys[i]->exit_mutex, NULL); // inicializa o mutex responsável por exit_cond
        pthread_cond_init(&args->toys[i]->exit_cond, NULL); // inicializa a condição de saída do brinquedo
        sem_init(&args->toys[i]->ready_semaphore, 0, 0); // inicializa o semáforo de controle de nova rodada do brinquedo em zero
        pthread_create(&threads_toys[i], NULL, turn_on, (void *)args->toys[i]); // cria uma thread por brinquedo chamando o método turn_on
    }
}

// Desligando os brinquedos
void close_toys(){
    // Finaliza as threads e estruturas de sincronização utilizadas em cada brinquedo
    for (int i = 0; i < n_toys; i++) {
        pthread_join(threads_toys[i], NULL);
        pthread_mutex_destroy(&array_toys[i]->n_clientes_mutex);
        sem_destroy(&array_toys[i]->capacity_semaphore);
        pthread_mutex_destroy(&array_toys[i]->enter_mutex);
        pthread_cond_destroy(&array_toys[i]->enter_cond);
        sem_destroy(&array_toys[i]->join_semaphore);
        pthread_mutex_destroy(&array_toys[i]->exit_mutex);
        pthread_cond_destroy(&array_toys[i]->exit_cond);
        sem_destroy(&array_toys[i]->ready_semaphore);
    }
    free(threads_toys); // desaloca o vetor de threads
}