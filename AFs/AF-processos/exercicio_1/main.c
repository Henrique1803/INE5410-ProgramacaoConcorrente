#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

//       (pai)
//         |
//    +----+----+
//    |         |
// filho_1   filho_2

// ~~~ printfs  ~~~
// pai (ao criar filho): "Processo pai criou %d\n"
//    pai (ao terminar): "Processo pai finalizado!\n"
//  filhos (ao iniciar): "Processo filho %d criado\n"

// Obs:
// - pai deve esperar pelos filhos antes de terminar!

void processoFilho() {
    printf("Processo filho %d criado\n", getpid());
}

void processoPai(int pid1) {
    printf("Processo pai criou %d\n", pid1);
}

int main(int argc, char **argv) {

    for (int i = 0; i < 2; i++) {
        pid_t pid1;
        pid1 = fork();

        if (pid1 == 0) {
            processoFilho();
            exit(0);
        } else {
            processoPai(pid1);
        }
    }

    while(wait(NULL) >= 0);
    printf("Processo pai finalizado!\n");

    return 0;
}
