//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/message_private.h"
#include "../include/data.h"
#include "../include/entry.h"
#include "../include/tree.h"
#include "../include/client_stub.h"
#include "../include/client_stub_private.h"
#include "../include/network_client.h"

struct rtree_t;

/* Função para estabelecer uma associação entre o cliente e o servidor,
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtree_t *rtree_connect(const char *address_port) {

	char *stuff = strtok(strdup(address_port), ":");

	char *ip = strdup(stuff);
	char *port = strdup(strtok(NULL, ":"));

	if (ip == NULL || port == NULL) {
		printf("[ERROR] Wrong ip or port!\n");
		return NULL;
	}

	struct rtree_t *rtree;
	rtree = malloc(sizeof(struct rtree_t));

	if (rtree == NULL) {
		free(stuff);
		free(ip);
		free(port);
		return NULL;
	}

	rtree->ip = ip;
	rtree->port = port;

	if ((network_connect(rtree)) == -1) {
		free(stuff);
		free(ip);
		free(port);
		free(rtree);
		return NULL;
	}

	free(stuff);
	free(ip);
	free(port);
	return rtree;

}

int rtree_verify(struct rtree_t *rtree, int op_n){
	if (rtree == NULL  || op_n < 0) return -1;

	struct message_t *msg_pedido;
	msg_pedido = malloc(sizeof (struct message_t));
	msg_pedido->message = malloc(sizeof (MessageT));

	message_t__init(msg_pedido->message);

	msg_pedido->message->opcode = 70; //OP_VERIFY
	msg_pedido->message->c_type = 50; //CT_RESULT
	msg_pedido->message->tree_info = op_n;

	struct message_t *msg_resposta;
	msg_resposta = malloc(sizeof (struct message_t));
	msg_resposta = network_send_receive(rtree, msg_pedido);
	free(msg_pedido);

	if (msg_resposta == NULL || msg_resposta->message->opcode == 99) {
		free(msg_resposta);
		return -1;
		}

	int returned = msg_resposta->message->tree_info;
	free(msg_resposta);
	return returned;
}

/* Termina a associação entre o cliente e o servidor, fechando a
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtree_disconnect(struct rtree_t *rtree){

	int output = network_close(rtree);
	//free(rtree); TIREI ISTO PARA TENTAR QUE FUNCIONE
	return output;

}

/* Função para adicionar um elemento na árvore.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtree_put(struct rtree_t *rtree, struct entry_t *entry) {

	if (rtree == NULL  || entry == NULL) return -1;

	struct message_t *msg_pedido;
	msg_pedido = malloc(sizeof (struct message_t));
	msg_pedido->message = malloc(sizeof (MessageT));

	message_t__init(msg_pedido->message);

	msg_pedido->message->opcode = 40; //OP_PUT
	msg_pedido->message->c_type = 30; //CT_ENTRY
	msg_pedido->message->datatsize = entry->value->datasize;
	msg_pedido->message->key = entry->key;
	msg_pedido->message->data = entry->value->data;

	struct message_t *msg_resposta;
	msg_resposta = malloc(sizeof (struct message_t));
	msg_resposta = network_send_receive(rtree, msg_pedido);
	free(msg_pedido);

	if (msg_resposta == NULL || msg_resposta->message->opcode == 99) {
		free(msg_resposta);
		return -1;
  	}

	if(msg_resposta->message->tree_info < 0){
		free(msg_resposta);
		printf("[ERROR] Couldn't add 'put' operation to the queue\n");
	}
	printf("Success! Your 'put' operation was added to the queue with id = %d!\n", msg_resposta->message->tree_info);
	free(msg_resposta);
	return 0;

}

/* Função para obter um elemento da árvore.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtree_get(struct rtree_t *rtree, char *key){

	if (rtree == NULL  || key == NULL) return NULL;

	struct message_t *msg_pedido;
	msg_pedido = malloc(sizeof (struct message_t));
	msg_pedido->message = malloc(sizeof (MessageT));

	message_t__init(msg_pedido->message);

	msg_pedido->message->opcode = 30; //OP_GET
	msg_pedido->message->c_type = 10; //CT_KEY
	msg_pedido->message->key = key;

	struct message_t *msg_resposta;
	msg_resposta = malloc(sizeof (struct message_t));
	msg_resposta = network_send_receive(rtree, msg_pedido);

	free(msg_pedido);

	if (msg_resposta == NULL || msg_resposta->message->opcode == 99) {
		free(msg_resposta);
		return NULL;
	}

	struct data_t *data_get;
	data_get = data_create2(msg_resposta->message->datatsize, msg_resposta->message->data);

	free(msg_resposta);
	return data_get;

}

/* Função para remover um elemento da árvore. Vai libertar
 * toda a memoria alocada na respetiva operação rtree_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtree_del(struct rtree_t *rtree, char *key){

	if(rtree == NULL  || key == NULL) return -1;

	struct message_t *msg_pedido;
	msg_pedido = malloc(sizeof (struct message_t));
	msg_pedido->message = malloc(sizeof (MessageT));

	message_t__init(msg_pedido->message);

	msg_pedido->message->opcode = 20; //OP_DEL
	msg_pedido->message->c_type = 10; //CT_KEY
	msg_pedido->message->key = key;

	struct message_t *msg_resposta;
	msg_resposta = malloc(sizeof (struct message_t));
	msg_resposta = network_send_receive(rtree, msg_pedido);
	free(msg_pedido);

	if(msg_resposta == NULL || msg_resposta->message->opcode == 99){
		free(msg_resposta);
		return -1;
	}

	if(msg_resposta->message->tree_info < 0){
		free(msg_resposta);
		printf("[ERROR] Couldn't add 'del' operation to the queue (DEL DE N EXISTE)\n");
	}
	printf("Success! Your 'del' operation was added to the queue with id = %d!\n", msg_resposta->message->tree_info);

	free(msg_resposta);
	return 0;

}

/* Devolve o número de elementos contidos na árvore.
 */
int rtree_size(struct rtree_t *rtree){

	if(rtree == NULL) return -1;

	struct message_t *msg_pedido;
	msg_pedido = malloc(sizeof (struct message_t));
	msg_pedido->message = malloc(sizeof (MessageT));

	message_t__init(msg_pedido->message);

	msg_pedido->message->opcode = 10; //OP_SIZE
	msg_pedido->message->c_type = 60; //CT_NONE

	struct message_t *msg_resposta;
	msg_resposta = malloc(sizeof (struct message_t));
	msg_resposta = network_send_receive(rtree, msg_pedido);
	free(msg_pedido);

	if(msg_resposta == NULL || msg_resposta->message->opcode == 99){
		free(msg_resposta);
		return -1;
	}

	return msg_resposta->message->tree_info;

}

/* Função que devolve a altura da árvore.
 */
int rtree_height(struct rtree_t *rtree){

	if(rtree == NULL) return -1;

	struct message_t *msg_pedido;
	msg_pedido = malloc(sizeof (struct message_t));
	msg_pedido->message = malloc(sizeof (MessageT));

	message_t__init(msg_pedido->message);

	msg_pedido->message->opcode = 60; //OP_HEIGHT
	msg_pedido->message->c_type = 60; //CT_NONE

	struct message_t *msg_resposta;
	msg_resposta = malloc(sizeof (struct message_t));
	msg_resposta = network_send_receive(rtree, msg_pedido);
	free(msg_pedido);

	if(msg_resposta == NULL || msg_resposta->message->opcode == 99){
		free(msg_resposta);
		return -1;
	}

	return msg_resposta->message->tree_info;

}

/* Devolve um array de char* com a cópia de todas as keys da árvore,
 * colocando um último elemento a NULL.
 */
char **rtree_get_keys(struct rtree_t *rtree){

	if(rtree == NULL) return NULL;

	struct message_t *msg_pedido;
	msg_pedido = malloc(sizeof (struct message_t));
	msg_pedido->message = malloc(sizeof (MessageT));

	message_t__init(msg_pedido->message);

	msg_pedido->message->opcode = 50; //OP_GETKEYS
	msg_pedido->message->c_type = 60; //CT_NONE

	struct message_t *msg_resposta;
	msg_resposta = malloc(sizeof (struct message_t));
	msg_resposta = network_send_receive(rtree, msg_pedido);
	free(msg_pedido);

	if(msg_resposta == NULL || msg_resposta->message->opcode == 99){
	free(msg_resposta);
	return NULL;
	}

	char **output;
	output = malloc(msg_resposta->message->n_keys * sizeof(char *) + 1);
	for(int i = 0; i < msg_resposta->message->n_keys; ++i) {
		output[i] = strdup(msg_resposta->message->keys[i]);
	}
	output[msg_resposta->message->n_keys] = NULL;

	return output;

}

/* Liberta a memória alocada por rtree_get_keys().
 */
void rtree_free_keys(char **keys){

	if(keys == NULL){
		return;
	}

	int i;
	for(i = 0; keys[i] != NULL; ++i){
		free(keys[i]);
	}
	free(keys);

}
