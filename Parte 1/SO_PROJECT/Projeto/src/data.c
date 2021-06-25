#include <stdlib.h>
#include <string.h>
#include "data.h"


/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size 
 */
struct data_t *data_create(int size){

	if(size<=0) {
		return NULL;
	}
	
	struct data_t *newStruct;
	newStruct = (struct data_t *) malloc(sizeof(struct data_t));
	
	if(newStruct == NULL) {	// se n conseguir alocar memoria
		return NULL; 
	}

	newStruct->datasize = size;
	newStruct->data = malloc(size+1); 	// +1, p causa do /0
	
	if(newStruct->data == NULL) {		// se n estiver nada no parametro data, dar free ah estrutura inutil
		free(newStruct);
		return NULL;
	}
	
	return newStruct;
}

/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data.
 */
struct data_t *data_create2(int size, void * data){
	
	struct data_t *newStruct = data_create(size);
	
	if(newStruct == NULL || data == NULL) {
		data_destroy(newStruct);
		return NULL;
	}
	
	free(newStruct->data);
	newStruct->data = data;
	return newStruct;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 */
void data_destroy(struct data_t *data){
	
	if(data == NULL) return; //se a estrutura nem existir
	
	if(data->data != NULL){
		free(data->data);
		free(data);
	}
	
}

/* Função que duplica uma estrutura data_t, reservando a memória
 * necessária para a nova estrutura.
 */
struct data_t *data_dup(struct data_t *data){
	
	if(data == NULL) {
		return NULL;
	}

	struct data_t *dataClone;
	dataClone = data_create(data->datasize);
	
	if (dataClone != NULL ) { // se memoria foi bem alocada
		if (data->data == NULL) {  // se data nulo, nada p copiar
			data_destroy(dataClone);
			return NULL;
		}
		memcpy(dataClone->data, data->data, (data->datasize) + 1);
		return dataClone;
	}
	
	return NULL; //se memoria nao foi bem alocada
}

/* Função que substitui o conteúdo de um elemento de dados data_t.
*  Deve assegurar que destroi o conteúdo antigo do mesmo.
*/
void data_replace(struct data_t *data, int new_size, void *new_data){
	//apagar campos da estrutura data 
	//colocar campos novos (argumentos)
	if(data != NULL && new_size >= 0 && new_data != NULL){
		data_destroy(data);
	}
	
	data->datasize = new_size;
	data->data = new_data;
}
