# imports do Python
from threading import Thread, Semaphore
import restaurant.shared as shared


"""
    Não troque o nome das variáveis compartilhadas, a assinatura e o nomes das funções.
"""
class Crew(Thread):
    
    """ Inicia o membro da equipe com um id (use se necessario)."""
    # atributo sem = semáforo para cada funcionário
    def __init__(self, id):
        super().__init__()
        self._id = id
        # Insira o que achar necessario no construtor da classe.
        self._sem = Semaphore(0)

    """ O membro da equipe espera um cliente. """
    # funcionários dão um release no semáforo tickets_sem do totem, de modo a ficarem bloqueados enquanto não houver nenhum cliente pra atender
    # quando um cliente retirar uma senha em totem, será dado um release nesse semáforo, de modo a liberar uma thread funcionária para atender esse cliente    
    def wait(self):
        print("O membro da equipe {} está esperando um cliente.".format(self._id))
        shared.totem.tickets_sem.acquire()


    """ O membro da equipe chama o cliente da senha ticket."""
    # realiza um for na lista de clientes, de modo a procurar o cliente que será atendido
    # verifica qual cliente da lista possui seu atributo ticket (senha) igual ao ticket que o funcionário recebe
    # quando encontra o cliente a ser atendido, da um release no semáforo do cliente, para que então o cliente faça seu pedido
    # e da um acquire no sem_atendimento, de modo a bloquear a thread funcionária até que o cliente faça seu pedido. quando o cliente realizar
    # o pedido, da um release no sem_atendimento, liberando o funcionário que estava bloqueado
    def call_client(self, ticket):
        print("[CALLING] - O membro da equipe {} está chamando o cliente da senha {}.".format(self._id, ticket))
        
        target_client = None
        for client in shared.clients:
            if client.ticket == ticket:
                target_client = client
                break
        target_client.sem.release()
        target_client.sem_atendimento.acquire()
    

    # funcionário adiciona o pedido (order) na fila de pedidos do chef, chamando chef.add_order(order), e logo em seguida
    # da um release no semáforo do chef, para que ele seja liberado para atender o pedido (caso esteja bloqueado), ou apenas continue
    # preparando os pedidos
    def make_order(self, order):
        print("[STORING] - O membro da equipe {} está anotando o pedido {} para o chef.".format(self._id, order))
        shared.chef.add_order(order)
        shared.chef.sem.release()

    """ Thread do membro da equipe."""
    # executa o método run em um loop, de modo que continue executando esse loop enquanto o restaurante estiver aberto
    # se o restaurante fechar (quando todos os clientes forem atendidos), seta-se o atributo close_restaurant pra True, e é dado um release
    # no semáforo de cada funcionário, liberando cada thread que estava bloqueada no método wait, entrando no if e quebrando o loop, e assim, 
    # encerrando a execução da thread. caso contrário, continua atendendo os clientes, pegando o menor ticket do totem, e removendo da lista
    # de chamadas do totem, de forma atômica (por isso usa o lock_call). depois disso, chama o cliente cuja senha é min_ticket, e depois,
    # realiza o pedido desse cliente
    def run(self):
        while True: # adicionar condição de parada posteriormente
            self.wait()
            if shared.close_restaurant:
                break
            with shared.totem.lock_call:
                min_ticket = min(shared.totem.call)
                shared.totem.call.remove(min_ticket)
            self.call_client(min_ticket)
            self.make_order(min_ticket)
        print(f"[LEAVE CREW] O funcionário {self._id} finalizou o expediente")


    # getters e setters dos atributos usados 
    @property
    def id(self):
        return self._id

    @id.setter
    def id(self, id):
        self._id = id

    @property
    def sem(self):
        return self._sem

    @sem.setter
    def sem(self, sem):
        self._sem = sem