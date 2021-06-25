//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/data.h"

/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size
 */
struct data_t *data_create(int size){

	if(size<=0) return NULL;

	struct data_t *new_data;
	new_data = (struct data_t *) malloc(sizeof(struct data_t));

	if(new_data == NULL) return NULL;

	new_data->data = (void *) malloc(size);
	new_data->datasize = size;

	return new_data;

}

/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data.
 */
struct data_t *data_create2(int size, void *data){

	struct data_t *new_data = data_create(size);

	if(new_data == NULL || data == NULL) {
		data_destroy(new_data);
		return NULL;
	}

	free(new_data->data);
	new_data->data = data;

	return new_data;

}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 */
void data_destroy(struct data_t *data){

	if(data == NULL) return;

	free(data->data);
	free(data);

}

/* Função que duplica uma estrutura data_t, reservando a memória
 * necessária para a nova estrutura.
 */
struct data_t *data_dup(struct data_t *data){

	if(data == NULL || data->data == NULL || data-> datasize < 0) return NULL;

	struct data_t *dataClone;
	dataClone = data_create(data->datasize);
	memcpy(dataClone -> data, data->data, data->datasize);

	return dataClone;

}

/* Função que substitui o conteúdo de um elemento de dados data_t.
*  Deve assegurar que destroi o conteúdo antigo do mesmo.
*/
void data_replace(struct data_t *data, int new_size, void *new_data){

	if(data == NULL || new_size < 0 || new_data == NULL) return;

	free(data->data);
	data->datasize = new_size;
	data->data = new_data;

}
