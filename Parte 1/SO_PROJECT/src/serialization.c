#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/data.h"
#include "../include/entry.h"
#include "../include/tree.h"

/* Serializa uma estrutura data num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho do buffer
 * alocado ou -1 em caso de erro.
 */
int data_to_buffer(struct data_t *data, char **data_buf){

  if(data == NULL || data_buf == NULL){
    return -1;
  }
  struct data_t *datta = data_dup(data);

  int data_buf_size = sizeof(datta->datasize) + datta->datasize;

  *data_buf = malloc(data_buf_size);

  //printf(" asdasdsadasd %d\n", data->datasize );

  memcpy(*data_buf, &datta->datasize, sizeof(datta->datasize));

  memcpy(*data_buf + sizeof(datta->datasize), &datta->data, sizeof(datta->data));

  //data_destroy(data);
  //free(data_buf);

  return data_buf_size;

}

/* De-serializa a mensagem contida em data_buf, com tamanho
 * data_buf_size, colocando-a e retornando-a numa struct
 * data_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct data_t *buffer_to_data(char *data_buf, int data_buf_size){

  if(data_buf == NULL || data_buf_size <= 0){
    return NULL;
  }

  int size = 0;
  memcpy(&size, data_buf, sizeof(int));

  struct data_t *datta = data_create(size);

  memcpy(&datta->data, data_buf + sizeof(int), size);
  //data_buf = NULL;

  return datta;

}

/* Serializa uma estrutura entry num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho deste
 * buffer ou -1 em caso de erro.
 */
int entry_to_buffer(struct entry_t *entry, char **entry_buf){

  if(entry == NULL || entry_buf == NULL){
    return -1;
  }

  int entry_buf_size = sizeof(int) + entry->value->datasize + strlen(entry->key) + 1 + sizeof('\0');
  *entry_buf = malloc(entry_buf_size);

  //por o datasize
  memcpy(*entry_buf, &entry->value->datasize , sizeof(int));

  //por o data
  memcpy(*entry_buf + sizeof(int), &entry->value->data, entry->value->datasize);

  //por a key
  memcpy(*entry_buf + sizeof(int) + entry->value->datasize, &entry->key, strlen(entry->key) + 1);

  return entry_buf_size;
}

/* De-serializa a mensagem contida em entry_buf, com tamanho
 * entry_buf_size, colocando-a e retornando-a numa struct
 * entry_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct entry_t *buffer_to_entry(char *entry_buf, int entry_buf_size){
  if(entry_buf == NULL || entry_buf_size <= 0){
    return NULL;
  }

  int size = 0;
  memcpy(&size, entry_buf, sizeof(int));
  //printf("!!!!!!!!!!!!!!!!!!!!  %d\n", size );

  struct data_t *datta = data_create(size);

  memcpy(&datta->data, entry_buf + sizeof(int), size);

  /*
  void *data_buffff;
  data_buffff = malloc(tamanh_data);
  memcpy(&data_buffff, entry_buf + sizeof(int), tamanh_data);
  printf("VOID PASSOU\n" );
  */
  char *chave = "";
  int size2 = entry_buf_size - sizeof('\0') - sizeof(int) - size + 1;
  //printf("%d\n", size2 );
  chave = (char *) malloc (size2 * sizeof(char*));
  //printf("PASSOU KEY MALLOC\n" );

  memcpy(&chave, entry_buf + sizeof(int) + size, entry_buf_size - sizeof('\0') - sizeof(int) - size + 1);

  //printf("%s\n", chave);

  //struct data_t *data_return = data_create2(tamanh_data,data_buffff);

  struct entry_t *entryyy = entry_create(chave, datta);

  //printf("NAO ACABOU\n" );
  //free(chave);
  //free(data_buffff);
  //entry_buf = NULL;

  return entryyy;
}

/* Serializa uma estrutura tree num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho deste
 * buffer ou -1 em caso de erro.
 */
int tree_to_buffer(struct tree_t *tree, char **tree_buf){
  return -1;
}

/* De-serializa a mensagem contida em tree_buf, com tamanho
 * tree_buf_size, colocando-a e retornando-a numa struct
 * tree_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct entry_t *buffer_to_tree(char *tree_buf, int tree_buf_size){
  return NULL;
}
