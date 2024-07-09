from time import sleep
from random import randint
from threading import Thread, Semaphore

def produtor():
  global buffer
  for i in range(10):
    sleep(randint(0,2))           # fica um tempo produzindo...
    item = 'item ' + str(i)
    # verifica se há lugar no buffer
    vazio.acquire()
    lock_prod.acquire()
    buffer.append(item)
    print('Produzido %s (ha %i itens no buffer)' % (item,len(buffer)))
    lock_prod.release()
    cheio.release()

def consumidor():
  global buffer
  for i in range(10):
    # aguarda que haja um item para consumir
    cheio.acquire()
    lock_cons.acquire()
    item = buffer.pop(0)
    print('Consumido %s (ha %i itens no buffer)' % (item,len(buffer)))
    lock_cons.release()
    vazio.release()
    sleep(randint(0,2))         # fica um tempo consumindo.../

buffer = []
tam_buffer = 3
# cria semáforos
cheio = Semaphore(0)
vazio = Semaphore(tam_buffer)
lock_prod = Semaphore(1)
lock_cons = Semaphore(1)

produtor = Thread(target=produtor) 
consumidor = Thread(target=consumidor) 
produtor.start()
consumidor.start()
produtor.join()
consumidor.join() 