//Grupo 62: Pedro Afonso Neves fc46430
//Ricardo Calçado fc46382
//Tiago Ferreira fc47895

#include <stdlib.h>
#include <string.h>

#include "data.h"

struct data_t data;

/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size
 */
struct data_t *data_create(int size){
  if (size < 0){
    return NULL;
  }
  struct data_t *p;
  p = (struct data_t *) malloc (sizeof (struct data_t));

  if (p == NULL){
    return NULL;
  }

  p->datasize = size;
  p->data = malloc(size);

  if (p->data == NULL) {
    free(p);
    return NULL;
  }
  return p;
}

/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data.
 */
struct data_t *data_create2(int size, void *data){
  if (size <= 0){
    return NULL;
  }

  if (data == NULL){
    return NULL;
  }

  struct data_t *p = data_create(size);

  if (p == NULL){
    return NULL;
  }
  memcpy(p->data,data,size);

  p->datasize = size;

  return p;
}

/* Função que destrói um bloco de dados e liberta toda a memória.
 */
void data_destroy(struct data_t *data){

  if (data != NULL){
     free(data->data);
     free(data);
  }

}

/* Função que duplica uma estrutura data_t.
 */
struct data_t *data_dup (struct data_t *data){
  if (data == NULL) {
    return NULL;
  }
  return data_create2(data->datasize,data->data);
}
