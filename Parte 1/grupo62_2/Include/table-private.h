//Grupo 62: Pedro Afonso Neves fc46430
//Ricardo Calçado fc46382
//Tiago Ferreira fc47895

#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "table.h"
#include "data.h"

struct table_t{
  int size;
  int numElem;
  struct entry_t *ary;
  int numCol;

};

/*
Retorna o numero de colisões que aconteceram na tabela
*/
int getCollisions(struct table_t *tabela);

/*
Retorna o tamanho da tabela
*/
int getSize(struct table_t *tabela);

#endif
