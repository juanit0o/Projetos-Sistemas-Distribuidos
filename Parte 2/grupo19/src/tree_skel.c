//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/data.h"
#include "../include/entry.h"
#include "../include/tree.h"
#include "../include/message_private.h"
#include "../include/tree_skel.h"


struct tree_t *tree;

/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke().
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init() {

	tree = tree_create();
	if (tree == NULL) {
		return -1;
	}
	return 0;

}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy(){

	tree_destroy(tree);
	tree = NULL;

}

/* Executa uma operação na árvore (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, árvore nao incializada)
*/
int invoke(struct message_t *msg) {

	if (tree == NULL) {
		msg->message->opcode = 99;
	}

	int size = 0;
	int height = 0;
	struct data_t *data_get;
	struct data_t *data_put;
	char **chaves;

	switch (msg->message->opcode) {
		case 0: //OP_BAD
			msg->message->c_type = 60;  //CT_NONE
			return -1;
			break;

		case 10: //OP_SIZE
			if ((size = tree_size(tree)) >= 0) {
				msg->message->opcode += 1;
				msg->message->c_type = 50;
				msg->message->tree_info = size;
				return 0;
			} else {
				msg->message->opcode = 99;
				msg->message->c_type = 60;
				return -1;
			}
			break;

		case 20: //OP_DEL
			if (tree_del(tree,msg->message->key) == 0) {
				msg->message->opcode += 1;
				msg->message->c_type = 60;
				return 0;
			} else {
				msg->message->opcode = 99;
				msg->message->c_type = 60;
				return 0;
			}
			break;

		case 30: //OP_GET
			if (msg->message->key == NULL) { //nao existe a key na arvore
				msg->message->opcode = 99;
				msg->message->c_type = 60;
				return -1;
			} else if ((data_get = tree_get(tree,msg->message->key)) != NULL) { //ver se a key existe
				msg->message->opcode += 1;
				msg->message->c_type = 20;
				msg->message->data = data_get->data;
				msg->message->datatsize = data_get->datasize;
				return 0;
			} else {
				msg->message->opcode += 1;
				msg->message->c_type = 20;
				msg->message->data = NULL;
				msg->message->datatsize = 0;
				return 0;
			}
			break;

		case 40: //OP_PUT
			data_put = data_create2(msg->message->datatsize, msg->message->data);
			if (tree_put(tree, msg->message->key, data_put) == 0) {
				msg->message->opcode += 1;
				msg->message->c_type = 60;
				return 0;
			} else {
				msg->message->opcode = 99;
				msg->message->c_type = 60;
				return -1;
			}
			break;

	case 50: //OP_GETKEYS
		if (tree_size(tree) <= 0 || (chaves = tree_get_keys(tree)) == NULL) {
			msg->message->opcode += 1;
			msg->message->c_type = 40;
			msg->message->n_keys = 0;
			msg->message->keys = NULL;
			return 0;
		} else {
			int counter;
			for (counter = 0; chaves[counter] != NULL; counter++) {}
			msg->message->n_keys = counter;
			msg->message->keys = chaves;
			msg->message->opcode += 1;
			msg->message->c_type = 40;
			return 0;
		}
		break;

	case 60: //OP_HEIGHT
		if ((height = tree_height(tree)) >= 0) {
			msg->message->opcode += 1;
			msg->message->c_type = 50;
			msg->message->tree_info = height;
			return 0;
		} else {
			msg->message->opcode = 99;
			msg->message->c_type = 60;
			return -1;
		}
		break;

	case 99: //OP_ERROR
		msg->message->opcode = 99;
		msg->message->c_type = 60;
		return -1;
		break;

	default:
		printf("Invalid method invoke\n");
		return -1;
		break;
	}
	return -1;

}
