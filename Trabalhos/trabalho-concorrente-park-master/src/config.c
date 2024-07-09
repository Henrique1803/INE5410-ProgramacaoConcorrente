/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"


/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

config_t parse(int argc, char **argv)
{
    int c;
    config_t config = {1, 5, 2, 4};

    while ((c = getopt(argc, argv, "c:t:g:s:h")) != -1)
    {
        switch (c) {
            case 'c': // Quantas pessoas?
                config.clients = atoi(optarg);
                break;
            case 't': // Quantos brinquedos?
                config.toys = atoi(optarg);
                break;
            case 'g': // Quantos bilheteiros?
                config.tickets = atoi(optarg);
                break;
            case 's': // Defina sua semente.
                config.seed = atoi(optarg);
                break;
            case 'h': // Imprima ajuda!
                printf("Usage: %s [OPTIONS] \n\nOptions:\n", argv[0]);
                printf("  -c  Numero de clientes (default 1).\n");
                printf("  -t  Numero de brinquedos (default 5).\n");
                printf("  -g  Numero de bilheterias (default 2).\n");
                printf("  -s  Seed (default 1).\n");
                printf("  -h  Prints this help.\n");
            default:
                exit(1);
        }
    }

    return config;
}
