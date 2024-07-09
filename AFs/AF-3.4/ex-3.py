from multiprocessing import Process, Queue

def origem(fila1):
  for i in range(10):
    msg = 'Teste ' + str(i)
    fila1.put(msg)
    print('Mensagem enviada:', msg)

def ponte(fila1, fila2):
  for i in range(10):
    msg = fila1.get()
    fila2.put(msg)
    print('Mensagem encaminhada:', msg)

def destino(fila2):
  for i in range(10):
    msg = fila2.get()
    print('Mensagem recebida:', msg)

if __name__ == '__main__':
  fila1 = Queue()
  fila2 = Queue()
  o = Process(target=origem, args=(fila1,))
  p = Process(target=ponte, args=(fila1,fila2))
  d = Process(target=destino, args=(fila2,))

  o.start()
  p.start()
  d.start()
  o.join()
  p.join()
  d.join()

  fila1.close()
  fila2.close()