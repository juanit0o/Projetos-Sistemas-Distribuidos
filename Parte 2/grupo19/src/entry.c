//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/entry.h"
#include "../include/data.h"

/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados passados.
 */
struct entry_t *entry_create(char *key, struct data_t *data){

	if(key == NULL) return NULL;

	struct entry_t *new_entry;
	new_entry = (struct entry_t *) malloc(sizeof(struct entry_t));

	if(new_entry == NULL) return NULL;

	new_entry->key = key;
	new_entry->value = data;

	return new_entry;

}

/* Função que inicializa os elementos de uma entry com o
 * valor NULL.
 */
void entry_initialize(struct entry_t *entry){

	if(entry == NULL) return;

	entry->key = NULL;
	entry->value = NULL;

}

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry){

	if(entry == NULL) return;

	if(entry->value != NULL) data_destroy(entry->value);

	if(entry->key != NULL) free(entry->key);

	free(entry);

}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t *entry){

	if(entry == NULL) return NULL;

	struct entry_t *new_entry = NULL;
	new_entry = (struct entry_t*) malloc(sizeof (struct entry_t));

	if (new_entry == NULL) return NULL;

	new_entry->key = strdup(entry->key);

	if (new_entry->key == NULL) {
		free(new_entry);
		return NULL;
	}

	new_entry->value = data_dup(entry->value);

	if (new_entry->value == NULL) {
		free(new_entry);
		return NULL;
	}

	return new_entry;

}

/* Função que substitui o conteúdo de uma entrada entry_t.
*  Deve assegurar que destroi o conteúdo antigo da mesma.
*/
void entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){

	if(entry == NULL || new_key == NULL) return;

	free(entry->key);
	entry->key = new_key;

	if(new_value != NULL){
		data_destroy(entry->value);
		entry->value = new_value;
	}

}

/* Função que compara duas entradas e retorna a ordem das mesmas.
*  Ordem das entradas é definida pela ordem das suas chaves.
*  A função devolve 0 se forem iguais, -1 se entry1<entry2, e 1 caso contrário.
*/
int entry_compare(struct entry_t *entry1, struct entry_t *entry2){

	int compStrc = strcmp(entry1->key, entry2->key);
	int compTamanho = strlen(entry1->key) - strlen(entry2->key);

	if(compTamanho == 0){
		return (compStrc == 0) ? 0 : (compStrc > 0 ? 1 : -1);
	} else if (compTamanho > 0){
		return 1;
	} else {
		return -1;
	}

}
