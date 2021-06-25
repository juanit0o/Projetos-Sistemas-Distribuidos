#include "entry.h"
#include "data.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados passados.
 */
struct entry_t *entry_create(char *key, struct data_t *data){
	
	if(data == NULL) {
		return NULL;
	}
	
	struct entry_t *new_entry;
	new_entry = (struct entry_t *) malloc(sizeof(struct entry_t));
	
	if(new_entry == NULL) {	// se n conseguir alocar memoria
		return NULL; 
	}

	new_entry->key = key;
	new_entry->value = data;
	
	return new_entry;
}

/* Função que inicializa os elementos de uma entry com o
 * valor NULL.
 */
void entry_initialize(struct entry_t *entry){
	if(entry != NULL){
		entry->key = NULL;
		entry->value = NULL;
	}
	return;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry){
	
	
	if(entry == NULL) return; //se a estrutura nem existir
	
	if(entry->value != NULL){
		data_destroy(entry->value);
	}
	if(entry->key != NULL){
		free(entry->key);
	}
	free(entry);
	
	/*
	if(entry != NULL){
		if(entry->value){
			
		}
		if(entry->key){
			free(entry->key);
		}
		free(entry);
	}*/
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t *entry){
	if(entry == NULL || entry->key == NULL || entry->value == NULL){
		return NULL;
	}
	struct entry_t *entry_clone;
	entry_clone = (struct entry_t *) malloc(sizeof(struct entry_t));
	if(entry_clone == NULL){
		return NULL;
	}

	//chamar o create_entry em vez disto maybe?
	
	int key_size = strlen(entry->key)+1; // p causa do \0
	entry_clone->key = (char*) malloc(key_size);
	if(entry_clone->key == NULL) {
		free(entry_clone);
		return NULL;
	}
	strcpy(entry_clone->key, entry->key);
	entry_clone->value = data_dup(entry->value);
	if(entry_clone->value == NULL) {
		free(entry_clone->key);
		free(entry_clone);
		return NULL;
	}
	return entry_clone;
}

/* Função que substitui o conteúdo de uma entrada entry_t.
*  Deve assegurar que destroi o conteúdo antigo da mesma.
*/
void entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){
	
	//apagar campos da estrutura entry 
	//colocar campos novos (argumentos)
	if(entry != NULL && new_key >= 0 && new_value != NULL){
		entry_destroy(entry);
	}
	
	entry->key = new_key;
	entry->value = new_value;
}

/* Função que compara duas entradas e retorna a ordem das mesmas.
*  Ordem das entradas é definida pela ordem das suas chaves.
*  A função devolve 0 se forem iguais, -1 se entry1<entry2, e 1 caso contrário.
*/
int entry_compare(struct entry_t *entry1, struct entry_t *entry2){
	
	if(atoi(entry1->key) == atoi(entry2->key)){
		return 0;
	}
	else if(atoi(entry1->key) < atoi(entry2->key)){
		return -1;
	}
	else{
		return 1;
	}
}


