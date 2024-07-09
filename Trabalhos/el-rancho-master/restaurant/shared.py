# Espaco reservado para voce inserir suas variaveis globais.
# Voce pode inserir como funcao (exemplo): 
# 
#  my_global_variable = 'Awesome value'
#  def get_my_global_variable():
#       global my_global_variable
#       return my_global_variable

# Decidimos não seguir a implementação com funções, somente com as variáveis mesmo
# Váriaveis globais utilizadasno projeto:

totem = None                # Variável global que vai conter o totem
table = None                # Variável que representa a grande mesa do restaurante
clients = list()            # Armazena as instâncias de clientes que estão no restaurante
clients_in_restaurant = 0   # Representa a quantidade de clientes dentro do restaurante
n_crew = 0                  # Representa a quantidade de funcionários
chef = None                 # Variável que representa o chef do restaurante
TIME_THINK_ORDER = 2        # Padrão de tempo para um cliente pensar no pedido que vai fazer (2 segundos)
TIME_EAT = 3                # Padrão de tempo para um cliente comer (3 segundos)
close_restaurant = False    # Variável responsável por sinalizar se o restaurante está fechado ou não

