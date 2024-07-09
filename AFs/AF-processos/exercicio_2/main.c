#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>


//                          (principal)
//                               |
//              +----------------+--------------+
//              |                               |
//           filho_1                         filho_2
//              |                               |
//    +---------+-----------+          +--------+--------+
//    |         |           |          |        |        |
// neto_1_1  neto_1_2  neto_1_3     neto_2_1 neto_2_2 neto_2_3

// ~~~ printfs  ~~~
//      principal (ao finalizar): "Processo principal %d finalizado\n"
// filhos e netos (ao finalizar): "Processo %d finalizado\n"
//    filhos e netos (ao inciar): "Processo %d, filho de %d\n"

// Obs:
// - netos devem esperar 5 segundos antes de imprmir a mensagem de finalizado (e terminar)
// - pais devem esperar pelos seu descendentes diretos antes de terminar

void processoNeto() {
    printf("Processo %d, filho de %d\n", getpid(), getppid());
    sleep(5);
    printf("Processo %d finalizado\n", getpid());  
}

void processoFilho() {
    printf("Processo %d, filho de %d\n", getpid(), getppid());
    for (int i = 0; i < 3; i++) {
        pid_t pid1;
        pid1 = fork();

        if (pid1 == 0) {
            processoNeto();
            exit(0);
        } 
    }

    while(wait(NULL) >= 0);
    printf("Processo %d finalizado\n", getpid());  
}

int main(int argc, char** argv) {

    for (int i = 0; i < 2; i++) {
        pid_t pid1;
        pid1 = fork();

        if (pid1 == 0) {
            processoFilho();
            exit(0);
        }
    }

    while(wait(NULL) >= 0);
    printf("Processo principal %d finalizado\n", getpid());    
    return 0;
}
