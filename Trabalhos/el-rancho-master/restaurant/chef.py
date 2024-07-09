# imports do Python
from threading import Thread, Semaphore, Lock
from time import sleep
from random import randint
import restaurant.shared as shared

"""
    Não troque o nome das variáveis compartilhadas, a assinatura e o nomes das funções.
"""
class Chef(Thread):
    
    # atributo queue_order = lista de pedidos
    # atributo sem = semáforo do chef, para ele saber se tem pedidos para ele atender ou não
    # atributo current_order = armazena a senha do pedido atual que o chefe está preparando
    # lock_queue = lock para realizar o acesso a fila de pedidos de forma atômica
    def __init__(self):
        super().__init__()
        # Insira o que achar necessario no construtor da classe.
        self._queue_order = list()  
        self._sem = Semaphore(0)    
        self._current_order = -1
        self._lock_queue = Lock()

    """ Método que adiciona o pedido na lista de pedidos do chefe """
    # utiliza um lock (mutex) antes de adicionar um pedido na fila de pedidos (queue_order) para realizar o acesso de forma atômica 
    def add_order(self, order):
        with self._lock_queue:  
            self._queue_order.append(order)

    """ Chef prepara um dos pedido que recebeu do membro da equipe."""
    # utiliza um lock (mutex) para retirar o pedido da fila de pedidos (queue_order) de forma atômica
    def cook(self):
        with self._lock_queue:  
            self._current_order = self._queue_order.pop(0)
        print("[COOKING] - O chefe esta preparando o pedido para a senha {}.".format(self._current_order))
        sleep(randint(1,5))

    """ Chef serve o pedido preparado."""
    # itera na lista de clientes, para encontrar o cliente cujo pedido está pronto, ou seja, verifica se o atributo ticket do cliente corresponde
    # ao pedido atual que o chefe preparou, que está armazenado no atributo current_order
    # quando encontra o cliente, da um release no semáforo dele, para que o cliente possa prosseguir sua execução indo até a mesa
    def serve(self):
        print("[READY] - O chefe está servindo o pedido para a senha {}.".format(self._current_order))
        target_client = None
        for client in shared.clients:
            if client.ticket == self._current_order:  
                target_client = client
                break
        target_client.sem.release()
        
    
    """ O chefe espera algum pedido vindo da equipe."""
    # chefe da um acquire no seu semáforo de modo a esperar que haja algum pedido para começar a cozinhar
    # quando houver um pedido, os funcionários (crew) irão dar um release nesse semáforo, de modo que o chefe continue sua execução
    # cozinhando e servindo os pedidos, enquanto houver pedidos
    def wait_order(self):
        print("O chefe está esperando algum pedido.")
        self._sem.acquire()


    """ Thread do chefe."""
    # chefe fica executando o laço enquanto o restaurante estiver aberto. Quando o restaurante for fechado, um acquire é dado em seu semáforo,
    # e seta-se a variável close_restaurant pra True. Com isso, a thread do chefe sai do método wait_order e entra no if, dando break no laço
    # e encerrando a thread. Caso contrário, o chefe irá continuar executando, preprando os pedidos e servindo os clientes
    def run(self):
        while True:
            self.wait_order()
            if shared.close_restaurant:
                break
            self.cook()
            self.serve()
        print("[LEAVE CHEF] O chefe finalizou o expediente")


    # getters e setters dos atributos usados
    @property
    def queue_order(self):
        return self._queue_order

    @queue_order.setter
    def queue_order(self, queue_order):
        self._queue_order = queue_order

    @property
    def sem(self):
        return self._sem

    @sem.setter
    def sem(self, sem):
        self._sem = sem

    @property
    def current_order(self):
        return self._current_order

    @current_order.setter
    def current_order(self, current_order):
        self._current_order = current_order

    @property
    def lock_queue(self):
        return self._lock_queue

    @lock_queue.setter
    def lock_queue(self, lock_queue):
        self._lock_queue = lock_queue