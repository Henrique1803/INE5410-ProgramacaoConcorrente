#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

typedef struct config {
    unsigned int clients;
    unsigned int tickets;
    unsigned int toys;
    unsigned int seed;
} config_t;

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

extern config_t config;
extern config_t parse(int argc, char **argv);

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/


#endif /*__CONFIG_H__*/