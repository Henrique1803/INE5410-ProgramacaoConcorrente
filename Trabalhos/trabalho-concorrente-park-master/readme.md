# Parallel-Park

A sua empresa foi contratada para desenvolver um sistema de simulação de parque de diversões que está sob estudos para ser construído na ilha de Florianópolis. Imagine uma grande atração que receba uma grande quantidade de pessoas e o quanto isso será benéfico para a população local!

Um programador de uma outra divisão, mas que é muito preguiçoso, começou a desenvolver o sistema. No entanto, logo alegou que ele não entendia nada de programação concorrente e resolveu abandonar o projeto (e a empresa). O gerente de projeto então designou o seu grupo para continuar a desenvolver a solução desse problema.

O programa é feito em C e deverá utilizar estruturas de sincronização para coordenar a concorrência e acesso às regiões críticas.

## **Visão Geral do Problema**

O parque será incrível. Ele contará com brinquedos e bilheterias ao qual os turistas devem atravessar. O fluxo inicial é o seguinte:

- Os clientes (implementado em `client.c`) devem entrar em uma **fila única** (implementação em `queue.c`) para acessar a bilheteria;
- Após ser chamado por algum funcionário da bilheteria (implementação em `tickets.c`) os clientes compram moedas para brincar nos brinquedos (implementação em `toy.c`). Assuma que todos os brinquedos possuem o mesmo valor (1 moeda) que é cobrado assim que o cliente acessa o brinquedo;
- Os clientes acessam os brinquedos do parque até que suas moedas acabem;
- O cliente vai embora quando não possui mais moedas. O parque não permite que o cliente compre moedas depois que ele acessou a parte interna, afinal, será mais caro contratar mais uma pessoa para vender moedas dentro do parque.
- Você pode controlar a quantidade máxima de moedas que um cliente eventualmente venha a adquirir alterando a macro `MAX_COINS` definida em `defs.h`.

Vamos falar agora sobre as principais estruturas do projeto.

## **Fila de Acesso à Bilheteria**

O programador deixou a estrutura da fila que dá acesso à bilheteria já implementada (pelo menos ele estudou estruturas de dados). A fila é uma lista ligada, em que um nó armazena um **valor inteiro** (que represenda um identificador de cliente) e o **próximo nó**. As estruturas de *enqueue*, *dequeue* e *is_queue_empty* enfileram, desenfileram e verificam se a fila está vazia ou não, respectivamente. A fila principal está implementada no arquivo no arquivo `queue.c` e deve ser única e exclusivamente acessada **pelas bilheterias** e **pelos clientes**.

## Bilheteria

A bilheteria (`tickets.c`) possui diversos funcionários que podem atender clientes. Cada funcionado da bilheteria é representado por uma _thread_ que, quando fica ociosa, chama um cliente e o retira da fila. Assim que todos os clientes são chamados, a bilheteria encerra sua execução e fecha as portas.

## Clientes

O parque é feito para os clientes! Cada cliente é representado por uma _thread_. O cliente deve esperar na fila da bilheteria e comprar suas moedas (a quantidade de moedas compradas é aleatória). Após acessar o parque, o cliente deve fazer o que veio fazer: brincar! Para isso, ele realiza as seguintes operações até que todas as suas moedas acabem:

1. Escolhe aleatoriamente um brinquedo;
2. Aguarda sua vez para ingressar no brinquedo escolhido;
3. Usa uma moeda para ingressar no brinquedo;
4. Permanece no brinquedo durante um tempo fixo até que a atração termine;
5. Sai do brinquedo e volta para o passo 1 (quando não tiver mais moedas sai do parque).

## **Brinquedos**

Os brinquedos são a atração do parque. Cada brinquedo é representado por uma _thread_ e possui uma capacidade máxima de lotação (você pode especificar o intervalo de capacidades possíveis utilizando as macros `MAX_CAPACITY_TOY` e `MIN_CAPACITY_TOY` no arquivo `defs.h`), afinal, segurança em primeiro lugar. Você deve garantir que o brinquedo não possua mais clientes do que a sua capacidade máxima definida quando a atração começar. Mas não se esqueça de que há brinquedos mais populares que outros. Se apenas 1 cliente brincar em um brinquedo você deve garantir que ele se divirta, mesmo que sozinho! Os brinquedos possuem um tempo máximo de duração, e depois desse tempo, os clientes devem sair. Obviamente, você sabe, mas não custa lembrar, que um cliente pode ir ao mesmo brinquedo mais de uma vez. Nesse parque nos importamos com a diversão.

Infelizmente não temos colaboradores suficientes para ordenar as filas. Logo, **nos brinquedos não há filas**: a ordem é de chegada. Clientes devem aguardar a sua vez para entrar no brinquedo quando ele estiver em funcionamento ou quando a capacidade máxima está esgotada. Se a capacidade máxima do brinquedo não for atingida após um determinado período de tempo, os clientes que já acessaram o brinquedo podem se divertir.

## **Estrutura do Código**

Falando mais especificamente do código, a estrutura é definida da seguinte maneira:
```
build/  											    
    ├─ makefile.config     // Auxiliares de compilação
    ├─ makefile.executable // Auxiliares de compilação
include/  											    
    ├─ client.h            // Funções usadas no cliente
    ├─ config.h            // Funções usadas para parse
    ├─ defs.h              // Definições das threads do programa
    ├─ queue.h             // Funções da fila
    ├─ shared.h            // Header para variáveis compartilhadas
    ├─ tickets.h           // Funções usadas na bilheteria
    ├─ toy.h               // Funções usadas no brinquedo
src/
    ├─ Makefile            // Auxiliar de compilação
    ├─ client.c            // Código do cliente
    ├─ config.c            // Código do parse
    ├─ queue.c             // Código da fila
    ├─ shared.c            // Variáveis compartilhadas
    ├─ tickets.c           // Código da bilheteria
    ├─ toy.c               // Código dos brinquedos
```

## Executando a simulação

Para compilar o programa, execute o seguinte comando no diretório principal:

```
make
```

Para limpar os binários, execute o seguinte comando no diretório principal:

```
make clean
```

Para executar a simulação, execute o seguinte comando no diretório principal: 

```
 ./program -c <clientes> -t <brinquedos> -g <funcionários da bileteria> -s <semente do gerador de números aleatórios>
```

Todos os parâmetros de entrada são números inteiros. O parâmetro `-s` serve para configurar o gerador de números aleatórios.

Para imprimir ajuda:

```
 ./program -h
```

## Critérios de avaliação

### (3/10) Execução concorrente de brinquedos:
* O esqueleto está fornecido em `toy.c`;
* Erros de sincronização e condições de corrida serão descontados;
* Nesse arquivo não é necessária a implementação de filas;
* A lógica que autoriza os clientes a entrarem e saírem no/do brinquedo **deve** ser implementada nesse arquivo;
* Espera-se a correta finalização das threads. 

### (3/10) Execução concorrente dos funcionários da bilheteria:
* O esqueleto está fornecido em `tickets.c`;
* Erros de sincronização e condições de corrida serão descontados;
* Solucões baseadas em "supermutexes" (uma única sincronização global) serão descontadas;
* A lógica que autoriza o cliente a entrar no parque **deve** ser implementada nesse arquivo;
* Espera-se a correta finalização das threads.

### (4/10) Execução concorrente de clientes:
* O esqueleto está fornecido em `client.c`;
* Erros de sincronização e condições de corrida serão descontados;
* Solucões baseadas em "supermutexes" (uma única sincronização global) serão descontadas;
* A lógica que faz o fluxo do cliente no parque **deve** ser implementada nesse arquivo;
* Espera-se a correta finalização das threads.

## Orientações finais
 1. É **obrigatório** o uso do código esqueleto fornecido;
 2. Todos prints adicionados no código **devem** ser realizados por meio da função `debug()` disponível na biblioteca `defs.h` (existem vários exemplos pelo código esqueleto);
 3. Não abusem de variáveis compartilhadas. Usem quando necessário, declarando **apenas** no arquivo `shared.c`. O seu código ficará mais organizado e com um maior encapsulamento;
 4. O código deverá ser **obrigatoriamente** bem comentado. Códigos mal comentados sofrerão deduções na nota final;
 5. **Explorem ao máximo o paralelismo**. Quando mal utilizadas, estruturas de sincronização deixam de ser um remédio e se tornam um veneno para o paralelismo. Usem essas estruturas com responsabilidade;
 6. Há prints espalhados e padronizados no código. O seu uso é **obrigatório** nas áreas indicadas;
 7. Há seções sinalizadas em que **não é possível a alteração do código**. Isso serve para que o código funcione corretamente para todos;
 8. É possível a implementação de sincronização de threads usando variáveis condicionais em C. Leia mais sobre isso [aqui](https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_cond_signal.html);
 9. Atentem-se para a correta manipulação de memória (exemplo: `malloc` sem `free`). O uso descuidado pode gerar comportamentos imprevisíveis e sua nota também poderá sofrer deduções,
 10. O uso de **espera ocupada** em qualquer parte do código receberá desconto.

Bom trabalho!
