# imports do Python
from threading import Thread, Semaphore
from time import sleep

# imports do projeto
import restaurant.shared as shared

"""
    Não troque o nome das variáveis compartilhadas, a assinatura e o nomes das funções.
"""
class Client(Thread):
    
    """ Inicializa o cliente."""
    def __init__(self, i):
        self._id = i
        super().__init__()
        # atributo ticket (referente à senha retirada), e os semáforos inicializados
        self._ticket = -1
        self._sem = Semaphore(0)
        self._sem_atendimento = Semaphore(0)

    """ Pega o ticket do totem."""
    def get_my_ticket(self):
        # o cliente pega o ticket chamando o totem de forma atômica
        with shared.totem.lock_call:
            self._ticket = shared.totem.get_ticket()
            print("[TICKET] - O cliente {} pegou o ticket {}.".format(self._id, self._ticket))

    """ Espera ser atendido pela equipe. """
    def wait_crew(self):
        print("[WAIT] - O cliente {} esta aguardando atendimento.".format(self._id))
        # enquanto aguarda atendimento, seu semáforo está bloqueado (desbloqueado por crew ao atendê-lo)
        self._sem.acquire()
    
    """ O cliente pensa no pedido."""
    def think_order(self):
        print("[THINK] - O cliente {} esta pensando no que pedir.".format(self._id))
        sleep(shared.TIME_THINK_ORDER)

    """ O cliente faz o pedido."""
    def order(self):
        print("[ORDER] - O cliente {} pediu algo.".format(self._id))


    """ Espera pelo pedido ficar pronto. """
    def wait_chef(self):
        print("[WAIT MEAL] - O cliente {} esta aguardando o prato.".format(self._id))
        # enquanto aguarda o prato, seu semáforo está bloqueado (desbloqueado por chef ao finalizar)
        self._sem.acquire()
    
    """
        O cliente reserva o lugar e se senta.
        Lembre-se que antes de comer o cliente deve ser atendido pela equipe, 
        ter seu pedido pronto e possuir um lugar pronto pra sentar. 
    """
    def seat_and_eat(self):
        # cliente fica bloqueado no semáforo que representa o número de lugares da mesa, ao adquiri-lo pode se sentar
        print("[WAIT SEAT] - O cliente {} esta aguardando um lugar ficar livre".format(self._id))
        shared.table._sem_seats.acquire()
        print("[SEAT] - O cliente {} encontrou um lugar livre e sentou".format(self._id))
        shared.table.seat(self)
        shared.table.leave(self)

    """ O cliente deixa o restaurante."""
    def leave(self):
        # ao sair do restaurante, o contador de clientes (global) é decrementado, permitindo validar se o restaurante encerrou seu funcionamento
        print("[LEAVE CLIENT] - O cliente {} saiu do restaurante".format(self._id))
        shared.clients_in_restaurant -= 1
        if shared.clients_in_restaurant == 0:
            shared.close_restaurant = True
            # ao encerrar, os crews e o chef devem ter seu semáforo desbloqueado (aqueles que aguardavam clientes)
            shared.totem.tickets_sem.release(n=shared.n_crew)
            shared.chef.sem.release()
    
    """ Thread do cliente """
    def run(self):
        # os métodos são chamados na ordem em que o cliente deve realizar suas ações no restaurante
        self.get_my_ticket()
        self.wait_crew()
        self.think_order()
        self.order()
        self._sem_atendimento.release()
        self.wait_chef()
        self.seat_and_eat()
        self.leave()
    
    # abaixo foram criados os métodos acessores (getters e setters) de cada atributo para garantir o encapsulamento da classe

    @property
    def id(self):
        return self._id

    @id.setter
    def id(self, id):
        self._id = id

    @property
    def ticket(self):
        return self._ticket

    @ticket.setter
    def ticket(self, ticket):
        self._ticket = ticket

    @property
    def sem(self):
        return self._sem

    @sem.setter
    def sem(self, sem):
        self._sem = sem

    @property
    def sem_atendimento(self):
        return self._sem_atendimento

    @sem_atendimento.setter
    def sem_atendimento(self, sem_atendimento):
        self._sem_atendimento = sem_atendimento