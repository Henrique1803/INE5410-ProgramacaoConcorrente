#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

typedef struct 
{
    int comeco;   
    int fim;
    double * a;
    double * b;
    double * c;
} Parametros;

void *soma_vetores(void *parametros) {
    Parametros *params = (Parametros *) parametros;

    for (int i = params->comeco; i < params->fim; i++) {
        params->c[i] = params->a[i] + params->b[i];
    }

    pthread_exit(NULL);
}

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// | 
// |         tamanho do vetor (usado <-----+
// |         como 2o retorno)              |
// v                                       v
double* load_vector(const char* filename, int* out_size);


// Avalia o resultado no vetor c. Assume-se que todos os ponteiros (a, b, e c)
// tenham tamanho size.
void avaliar(double* a, double* b, double* c, int size);


int main(int argc, char* argv[]) {
    // Gera um resultado diferente a cada execução do programa
    // Se **para fins de teste** quiser gerar sempre o mesmo valor
    // descomente o srand(0)
    srand(time(NULL)); //valores diferentes
    //srand(0);        //sempre mesmo valor

    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n",
               argv[0]);
        return 1;
    }
  
    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }
    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    
    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }
    //Cria vetor do resultado 
    double* c = malloc(a_size*sizeof(double));

    // Calcula com uma thread só. Programador original só deixou a leitura 
    // do argumento e fugiu pro caribe. É essa computação que você precisa 
    // paralelizar
    //for (int i = 0; i < a_size; ++i) 
        //c[i] = a[i] + b[i];
    
    if (a_size < n_threads){
        printf("Número de threads excedido! O número de threads será igual ao tamanho do array(%d)!!!\n", a_size);
        n_threads = a_size;
    }

    pthread_t th[n_threads];
    Parametros params[n_threads];
    int tamanho_p_cada_thread = a_size / n_threads; // Tamanho de cada pedaço do vetor para cada thread (divisão inteira)

    for (int i = 0; i < n_threads; i++) {
        params[i].comeco = i * tamanho_p_cada_thread; // Cálculo do menor valor do intervalo
        if (i == n_threads - 1) { // Caso esteja na última thread 
            // fim recebe a_size, para caso n_threads e a_size não sejam múltiplos
            // Assim, a última thread (caso seja necessário) ficará com uma iteração a mais para fazer
            params[i].fim = a_size;  
        } else {
            params[i].fim = (i + 1) * tamanho_p_cada_thread; // Cálculo para o maior valor do intervalo
        }
        params[i].a = a;
        params[i].b = b;
        params[i].c = c;
        pthread_create(&th[i], NULL, soma_vetores, (void *)&params[i]);
    }

    for(int i = 0; i < n_threads; i++)
        pthread_join(th[i], NULL); // Espera o término de todas as outras threads 

    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, c, a_size);
    

    //Importante: libera memória
    free(a);
    free(b);
    free(c);

    return 0;
}


/*
O speedup obtido está proximo do speedup ideal?
Não, o speedup obtido resultou em 1, visto que o tempo de execução sequencial
foi idêntico ao tempo de execução utilizando 8 threads, 1 para cada núcleo. Com
isso, o speedup ideal seria um speedup de 8, ou muito próximo a 8.

O programa escala, ou seja, o speedup aumenta se aumentarmos o número de threads?
Não, o programa não escala. Por mais que o número de threads aumente, o speedup não
aumenta, visto que o notebook utilizado só possui 8 núcleos, e portanto, a partir
de 8 threads, não é possível rodar todas em paralelo.
*/