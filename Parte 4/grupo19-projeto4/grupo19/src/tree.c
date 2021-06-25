//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/data.h"
#include "../include/entry.h"
#include "../include/tree.h"
#include "../include/tree-private.h"

struct tree_t; /* A definir pelo grupo em tree-private.h */

/* Função para criar uma nova árvore tree vazia.
 * Em caso de erro retorna NULL.
 */
struct tree_t *tree_create(){

	struct tree_t *new_tree;
	new_tree = (struct tree_t *) malloc(sizeof(struct tree_t));

	new_tree->node = NULL;
	new_tree->left = NULL;
	new_tree->right = NULL;

	return new_tree;

}

/* Função para libertar toda a memória ocupada por uma árvore.
 */
void tree_destroy(struct tree_t *tree){

	if(tree != NULL){
		entry_destroy(tree->node);
		tree_destroy(tree->left);
		tree_destroy(tree->right);
	}
	free(tree);

}

/* Função para adicionar um par chave-valor à árvore.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na árvore,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int tree_put(struct tree_t *tree, char *key, struct data_t *value){

	if (key == NULL) return -1;

	if (tree == NULL) {
		tree = tree_create();
	}

	if (tree->node == NULL){
		tree->node = entry_create(strdup(key), data_dup(value));
		return 0;

	} else {

		int result = 0;
		if(strlen(key) - strlen(tree->node->key) == 0){
			result = (strcmp(key, tree->node->key) == 0) ? 0 : (strcmp(key, tree->node->key) > 0 ? 1 : -1);
		} else if (strlen(key) - strlen(tree->node->key) > 0){
			result =  1;
		} else {
			result = -1;
		}

		if (result == 0){
			data_destroy(tree->node->value);
			tree->node->value = data_dup(value);
			return 0;

		} else if (result > 0) {
			//DIREITA
			if(tree->right == NULL) {
        			tree->right = tree_create();
        			return tree_put(tree->right, key, value);
      			}

			return tree_put(tree->right, key, value);

		} else {
			//ESQUERDA
			if(tree->left == NULL) {
				tree->left = tree_create();
			}
			return tree_put(tree->left, key, value);
		}
	}
	return -1;

}

/* Função para obter da árvore o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser
 * libertados no contexto da função que chamou tree_get, ou seja, a
 * função aloca memória para armazenar uma *CÓPIA* dos dados da árvore,
 * retorna o endereço desta memória com a cópia dos dados, assumindo-se
 * que esta memória será depois libertada pelo programa que chamou
 * a função.
 * Devolve NULL em caso de erro.
 */
struct data_t *tree_get(struct tree_t *tree, char *key){

	if (tree == NULL || tree->node == NULL || key == NULL || tree->node->key == NULL){
		return NULL;
	}

	int result = 0;
	if(strlen(key) - strlen(tree->node->key) == 0){
		result = (strcmp(key, tree->node->key) == 0) ? 0 : (strcmp(key, tree->node->key) > 0 ? 1 : -1);
	} else if (strlen(key) - strlen(tree->node->key) > 0){
		result =  1;
  	} else {
		result = -1;
	}

	if (result == 0){
		return data_dup(tree->node->value);

	} else if (result > 0) {
		//DIREITA
		if (tree->right == NULL){
			return NULL;
		}
		return tree_get(tree->right, key);

	} else {
		//ESQUERDA
		if (tree->left == NULL){
			return NULL;
		}
		return tree_get(tree->left, key);
	}

}

/* Função para remover um elemento da árvore, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação tree_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int tree_del(struct tree_t *tree, char *key){

	if (tree == NULL || tree->node == NULL){
		return -1;
	}

	int result = 0;
	if(strlen(key) - strlen(tree->node->key) == 0){
		result = (strcmp(key, tree->node->key) == 0) ? 0 : (strcmp(key, tree->node->key) > 0 ? 1 : -1);
	} else if (strlen(key) - strlen(tree->node->key) > 0){
		result =  1;
	} else {
		result = -1;
	}

	if(result == 0){

		if(tree->left == NULL && tree->right == NULL){
			entry_destroy(tree->node);
			tree->node = NULL;
			return 0;

		}else if (tree->left == NULL){
			entry_destroy(tree->node);
			memcpy(tree, tree->right, sizeof(struct tree_t));
			tree->node = NULL;
			return 0;

		} else if (tree->right == NULL){
			entry_destroy(tree->node);
			memcpy(tree, tree->left, sizeof(struct tree_t));
			tree->node = NULL;
			return 0;

		} else {
			struct tree_t *treepoint = tree->right;
			while (treepoint != NULL && treepoint->left != NULL) {
				treepoint = treepoint->left;
			}
			tree->node = treepoint->node;
			return 0;

		}


	} else if(result < 0){
		//esquerda
		return tree_del(tree->left, key);

	} else {
		//direita
		return tree_del(tree->right, key);

	}
	return -1;

}

/* Função que devolve o número de elementos contidos na árvore.
 */
int tree_size(struct tree_t *tree){

	if(tree != NULL){
		if(tree->node == NULL) {
			return 0;
		}
		return 1 + tree_size(tree->left) + tree_size(tree->right);
	}
	return 0;

}

/* Função que devolve a altura da árvore.
 */
int tree_height(struct tree_t *tree){

	if(tree == NULL || tree->node == NULL){
		return 0;
	}

	int left_height = tree_height(tree->left);
	int right_height = tree_height(tree->right);

	return 1 + ((left_height > right_height) ? left_height : right_height);

}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * árvore, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **tree_get_keys(struct tree_t *tree){

	if(tree == NULL || tree_size(tree) == 0){
		return NULL;
	}

	char **keys = (char **) malloc((sizeof(char *) * sizeof(tree_size(tree))) + sizeof(NULL));

	int index = 0;

	keys[index] = strdup(tree->node->key);
	index += 1;

	if(tree->left != NULL){
		char **keysleft = tree_get_keys(tree->left);
		for(int i = 0; keysleft[i] != NULL; ++i){
			keys[index] =  strdup(keysleft[i]);
			index += 1;
		}
		tree_free_keys(keysleft);
	}

	if(tree->right != NULL){
		char **keysright = tree_get_keys(tree->right);
		for(int i = 0; keysright[i] != NULL; ++i){
			keys[index] =  strdup(keysright[i]);
			index += 1;
		}
		tree_free_keys(keysright);
	}
	keys[index] = NULL;

	return keys;

}

/* Função que liberta toda a memória alocada por tree_get_keys().
 */
void tree_free_keys(char **keys){

	int i;
	for(i = 0; keys[i] != NULL; ++i){
		free(keys[i]);
	}
	free(keys);

}
