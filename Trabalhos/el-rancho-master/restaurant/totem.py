# imports do Python
from random import randint
from threading import Semaphore, Lock

# imports do projeto
import restaurant.shared as shared


"""
    Não troque o nome das variáveis compartilhadas, a assinatura e o nomes das funções.
"""
class Totem:

    def __init__(self, number_of_clients):
        super().__init__()
        self.already_sampled = list()
        self.maximum_ticket_number = number_of_clients * 5
        self.call = list()
        # inicializa o semáforo e o mutex que controlam a sincronização e exclusão mútua relacionados ao totem
        self.tickets_sem = Semaphore(0)
        self.lock_call = Lock()

    """ 
        A função get_ticket não pode ser alterada. 
        Ela garante que um ticket aleatório (não repetido) seja criado e que a equipe seja chamada para atendê-lo.
    """
    def get_ticket(self):
        
        # Gera um ticket aleatório
        ticket_number = randint(1, self.maximum_ticket_number)   

        # Garante que o ticket não foi chamado anteriormente
        while ticket_number in self.already_sampled:
            ticket_number = randint(1, self.maximum_ticket_number)
        self.already_sampled.append(ticket_number)

        # Adiciona o ticket na lista de chamadas
        self.call.append(ticket_number)

        self.call_crew()

        return ticket_number    

    """ Insira sua sincronização."""
    def call_crew(self):
        # ao chamar crew, o semáforo de tickets é incrementado (release) permitindo desbloquear um dos crews para realizar o atendimento
        print("[CALLING] - O totem chamou a equipe para atender o pedido da senha {}.".format(self.already_sampled[-1]))
        self.tickets_sem.release()

