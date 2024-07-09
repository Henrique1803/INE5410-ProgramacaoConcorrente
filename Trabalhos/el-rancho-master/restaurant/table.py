from threading import Semaphore
import restaurant.shared as shared
from time import sleep
"""
    Não troque o nome das variáveis compartilhadas, a assinatura e o nomes das funções.
"""
class Table:

    """ Inicia a mesa com um número de lugares """
    def __init__(self,number):
        self._number = number
        # Insira o que achar necessario no construtor da classe.
        self._sem_seats = Semaphore(number) # Semáforo responsável por gerenciar o número de assentos disponíveis na mesa
    
    """ O cliente se senta na mesa."""
    def seat(self, client):
        # Cliente senta na mesa e fica comendo por um tempo (definido nas variáveis globais)
        print("[EAT] - O cliente {} está comendo".format(client.id))
        sleep(shared.TIME_EAT)
    
    """ O cliente deixa a mesa."""
    def leave(self, client):
        # Representa o comportamento do cliente saindo da mesa
        print("[LEAVE TABLE] - O cliente {} terminou de comer e saiu da mesa".format(client.id))
        self._sem_seats.release() # Libera um assento da mesa

    # Getters e setters da classe

    @property
    def number(self):
        return self._number

    @number.setter
    def number(self, number):
        self._number = number

    @property
    def sem_seats(self):
        return self._sem_seats

    @sem_seats.setter
    def sem_seats(self, sem_seats):
        self._sem_seats = sem_seats