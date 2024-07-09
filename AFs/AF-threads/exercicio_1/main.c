#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

//                 (main)      
//                    |
//    +----------+----+------------+
//    |          |                 |   
// worker_1   worker_2   ....   worker_n


// ~~~ argumentos (argc, argv) ~~~
// ./program n_threads

// ~~~ printfs  ~~~
// main thread (após término das threads filhas): "Contador: %d\n"
// main thread (após término das threads filhas): "Esperado: %d\n"

// Obs:
// - pai deve criar n_threds (argv[1]) worker threads 
// - cada thread deve incrementar contador_global (operador ++) n_loops vezes
// - pai deve esperar pelas worker threads  antes de imprimir!


int contador_global = 0;

void * incrementador(void *arg) {
    int loop_count = *((int *)arg); // Cast e desreferência do argumento para obter o número de iterações
    for (int i = 0; i < loop_count; i++) {
        contador_global++;
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("n_threads é obrigatório!\n");
        printf("Uso: %s n_threads n_loops\n", argv[0]);
        return 1;
    }

    int n_threads = atoi(argv[1]);
    int n_loops = atoi(argv[2]);

    pthread_t th[n_threads];

    for(int i = 0; i < n_threads; i++)
        pthread_create(&th[i], NULL, incrementador, (void *) &n_loops); // Cria n_loops threads
    for(int i = 0; i < n_threads; i++)
        pthread_join(th[i], NULL); // Espera o término de todas as outras threads 
    
    printf("Contador: %d\n", contador_global);
    printf("Esperado: %d\n", n_threads*n_loops);
    return 0;
}

/*
Qual a relação entre n_threads e a diferença entre "Contador" e "Esperado"?

A relação é que basicamente o aumento na diferença se dá de forma proporcinal 
ao aumento do número de Threads. Isso porque quanto mais threads, mais condições
de corrida irão ocorrer, e portanto, ocorrerá maior perda de incremento devido a
sobrescrita do contador.

Qual a relação entre n_loops e a diferença entre "Contador" e "Esperado"?

A relação se dá de forma parecida ao aumento do número de threads. Isso porque 
aumentando o número de loops, há um aumento na chance de ocorrer a condição de
corrida, pois as threads irão ficar mais tempo dentro do loop onde há a região
crítica.

Existe alguma regra ou padrão na diferença entre "Contador" e "Esperado"?

Há um comportamento não determinístico quando se fala de condição de corrida
e múltiplas threads acessando ao mesmo tempo uma determinada região crítica.
No entanto, é de se esperar que com o aumento do número de loops e número de 
threads disputando uma mesma região crítica, a diferença entre contador e 
esperado tende a aumentar, justamente pelo aumento das chances de ocorrer uma
sobrescrita do contador.
*/
