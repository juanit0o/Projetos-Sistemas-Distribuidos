//Grupo 62: Pedro Afonso Neves fc46430
//Ricardo Calçado fc46382
//Tiago Ferreira fc47895

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "table-private.h"
#include "table.h"
#include "entry.h"
#include "data.h"

//extern struct entry_t Entry;
struct table_t;

/*
 * Função para criar um hashValue
 */



int hash(char* key,int size) {

  if (key == NULL ||  size < 0)
    return -1;

    int sum = 0;
    int tamanho = strlen(key);

    if (size == 0)
      return 0;// verificar se é null ou outra cena qq
  if (tamanho<5) {
     for (int i = 0; i < tamanho; i++) {
       sum = sum + key[i];
     }

   }else {
     sum = key[0]+key[1]+key[tamanho-2]+key[tamanho-1];
   }
   return sum % size;
 }




/* Função para criar/inicializar uma nova tabela hash, com n
 * linhas(n = módulo da função hash)
 */
struct table_t *table_create(int n){

  struct table_t *tbl;

  if (n < 1)
    return NULL;

  tbl = (struct table_t *) malloc (sizeof (struct table_t));
  if (tbl == NULL)
    return NULL;

  tbl->ary = (struct entry_t *) malloc(sizeof (struct entry_t) *n);   //printf("malloc: %lu\n",sizeof(struct entry_t) *n);
                                                                    //  printf("table_create(%i): sizeof tbl=%lu   &   tbl->ary=%lu\n",n,sizeof(tbl),sizeof(*(tbl->ary)));
  if (tbl->ary == NULL) {
    free(tbl);
    return NULL;
  }

  tbl->size = n;
  tbl->numElem = 0;
  tbl->numCol = 0;

  int i;
  for(i=0; i < n; i++){
    entry_initialize(&(tbl->ary[i]));                                  // if(i<10 || i>1020) printf("i=%i  - NULL\n",i);
  }

  return tbl;
}

/* Libertar toda a memória ocupada por uma tabela.
 */
void table_destroy(struct table_t *table){

     if (table != NULL) {
       int i;
       for (i = 0; i < table->size; i++) {
        data_destroy(table->ary[i].value);
        free(table->ary[i].key);
       }
       free(table->ary);
       free(table);
     }
}

/* Função para adicionar um par chave-valor na tabela.
 * Os dados de entrada desta função deverão ser copiados.
 * Devolve 0 (ok) ou -1 (out of memory, outros erros)
 */
int table_put(struct table_t *table, char *key, struct data_t *value){


 // Verificar se a table, data, value existem
  if(table == NULL || value == NULL || key == NULL || table->numElem == table->size )
     return -1;

 //Criar o valor hash para ser colocado na tabela
  int hashIndex = hash(key,table->size);
  struct data_t *sub = table_get(table,key);
  struct entry_t *last_entry = &(table->ary[hashIndex]);

  if(sub != NULL){
    data_destroy(sub);
    return -1;
  }
//sem colisão
    if(last_entry->value == NULL){
      table->ary[hashIndex].key = strdup(key);
      table->ary[hashIndex].value = data_dup(value);
      table->numElem++;
      return 0;
    }
    //com colisão
    else{
      table->numCol = table->numCol+1;
      while(last_entry->next != NULL){
            last_entry = last_entry->next;
     }
     int i;
     for (i = table->size-1; i >= 0 ; i--) {
       if(table->ary[i].key == NULL){
         table->ary[i].key = strdup(key);
         table->ary[i].value = data_dup(value);
         table->numElem++;
         last_entry->next = &(table->ary[i]);
         return 0;
       }
     }
    }
    return -1;
}
/* Função para substituir na tabela, o valor associado à chave key.
 * Os dados de entrada desta função deverão ser copiados.
 * Devolve 0 (OK) ou -1 (out of memory, outros erros)
 */
int table_update(struct table_t *table, char *key, struct data_t *value){


if(table == NULL || key == NULL || value == NULL)
  return -1;

  int hashValue = hash(key,table->size);

  if(table->ary[hashValue].key == NULL){
    return -1;
  }

  struct entry_t *last_entry = &(table->ary[hashValue]);

  while (last_entry != NULL && strcmp(key,last_entry->key) != 0 ) {
    last_entry = last_entry->next;
  }

  if(strcmp(key,last_entry->key) == 0){
    data_destroy(last_entry->value);
    last_entry->value=data_dup(value);
  }

return 0;

}

/* Função para obter da tabela o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser libertados
 * no contexto da função que chamou table_get.
 * Devolve NULL em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key){                 // printf("===> search key: %s \n",key);


  //Verificar se a table não é null
     if(table == NULL)                                                 // printf("table is NULL\n");
         return NULL;

     //Verificar se a key não é null
     if(key == NULL)
         return NULL;

    int hashValue = hash(key,table->size);

     struct entry_t *nextData = &(table->ary[hashValue]);

     if(nextData->key == NULL){
       return NULL;
     }
       while(strcmp(nextData->key,key) != 0) {
         if(nextData->next != NULL){
           nextData = nextData->next;
         }
         else{
           return NULL;
         }
      }

    return data_dup((nextData->value));;
}

/* Devolve o número de elementos na tabela.
 */
int table_size(struct table_t *table){

  if (table == NULL) {
    return 0;
  }

  return table->numElem;
}

/* Devolve um array de char * com a cópia de todas as keys da tabela,
 * e um último elemento a NULL.
 */
char **table_get_keys(struct table_t *table){
  char **keyArray = (char**) malloc(sizeof(char*) * (table->numElem+1));
  if (keyArray == NULL) return NULL;
    int i;
    int k=0;
    for (i = 0; i < table->size; i++) {
      if (table->ary[i].key != NULL){
        keyArray[k] = strdup(table->ary[i].key);
        k++;
      }
    }
    keyArray[k] = NULL;
    return keyArray;
}
/* Liberta a memória alocada por table_get_keys().
 */
void table_free_keys(char **keys){
  int i = 0;
  if (keys == NULL)
   return;
  while(keys[i]!=NULL){
      free(keys[i]);
      i++;
  }
  if(keys!=NULL)
    free(keys);
}
/*
 Função para retornar o numero de Colisões
 0 = OK, -1 caso haja erro
*/
int getCollisions(struct table_t *tabela){
  int num = 0;
  if(tabela == NULL){
    return -1;
  }
  num = tabela->numCol;
  return num;
}

/*
 Função para retornar o numero de Colisões
 0 = OK, -1 caso haja erro
*/
int getSize(struct table_t *tabela){
  int len = 0;
  if(tabela == NULL){
    return -1;
  }
  len = tabela->size;
  return len;
}
