//Grupo 62: Pedro Afonso Neves fc46430
//Ricardo Calçado fc46382
//Tiago Ferreira fc47895

#include <stdlib.h>
#include <string.h>

#include "entry.h"
#include "data.h"


/* Função que inicializa os membros de uma entrada na tabela com
   o valor NULL.
 */
void entry_initialize(struct entry_t *entry){

  if(entry == NULL)
    return;


    entry->key = NULL;
    entry->value = NULL;
    entry->next = NULL;



}
/* Função que duplica um par {chave, valor}.
 */
struct entry_t *entry_dup(struct entry_t *entry){

  if (entry == NULL) {
    return NULL;
  }

  struct entry_t *ent;

  ent = (struct entry_t *) malloc (sizeof (struct entry_t));

  ent->key = strdup (entry->key);
  ent->value = data_dup(entry->value);
  ent->next = entry->next;

  return ent;
}
