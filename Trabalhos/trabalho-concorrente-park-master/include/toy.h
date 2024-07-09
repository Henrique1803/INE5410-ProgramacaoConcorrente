#ifndef __TOY_H__
#define __TOY_H__

#include <defs.h>

// Você pode declarar novas funções aqui

void access_toy(toy_t *self, int client_id);

/********************************
 *          ATENÇÃO             *
 *   NÃO EDITAR ESSAS FUNÇÕES   *
 *          ATENÇÃO             *
 *******************************/
void open_toys(toy_args *args);
void close_toys();

#endif