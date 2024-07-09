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
    double resultado;
} Parametros;

void *produto_escalar(void *parametros) {
    Parametros *params = (Parametros *) parametros;
    double result = 0;
    for (int i = params->comeco; i < params->fim; i++) {
        result += params->a[i] * params->b[i];
    }
    params->resultado = result;
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


// Avalia se o prod_escalar é o produto escalar dos vetores a e b. Assume-se
// que ambos a e b sejam vetores de tamanho size.
void avaliar(double* a, double* b, int size, double prod_escalar);

int main(int argc, char* argv[]) {
    srand(time(NULL));

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

    //Calcula produto escalar. Paralelize essa parte
    //double result = 0;
    //for (int i = 0; i < a_size; ++i) 
        //result += a[i] * b[i];

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
        pthread_create(&th[i], NULL, produto_escalar, (void *)&params[i]);
    }

    double result = 0;
    for (int i = 0; i < n_threads; i++) {
        pthread_join(th[i], NULL); // Espera o término de todas as outras threads
        result += params[i].resultado;
    }
    
    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, a_size, result);

    //Libera memória
    free(a);
    free(b);

    return 0;
}
