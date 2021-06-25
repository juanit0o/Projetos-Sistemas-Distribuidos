//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "../include/data.h"
#include "../include/entry.h"
#include "../include/tree.h"
#include "../include/message_private.h"
#include "../include/tree_skel.h"


struct tree_t *tree;
int last_assigned;
int op_count;
struct task_t *queue_head; //primeira tarefa a ser realizada, e la dentro tera um apontador para a proxima tarefa a ser executada (fila)
pthread_t processPedidos;

//lock e variavel condicional
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tree_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;

/* Inicia o skeleton da árvore.
* O main() do servidor deve chamar esta função antes de poder usar a
* função invoke().
* Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
*/
int tree_skel_init() {

	tree = tree_create();
	queue_head = malloc(sizeof(struct task_t));

	if (tree == NULL) {
		return -1;
	}

	last_assigned = 0; //contadores da fase 3 iniciados a 0
	op_count = 0;

	if(pthread_create(&processPedidos, NULL, &process_task, NULL) != 0){
		printf("[ERROR] Couldn't create consumer thread\n");
		return -1;
	}

	if(pthread_mutex_init(&queue_lock, NULL) < 0){
		printf("[ERROR] Couldn't initialize queue lock\n");
		return -1;
	}

	if(pthread_mutex_init(&tree_lock, NULL) < 0){
		printf("[ERROR] Couldn't initialize tree lock\n");
		return -1;
	}

	if(pthread_cond_init(&queue_not_empty, NULL) < 0){
		printf("[ERROR] Couldn't initialize tree lock\n");
		return -1;
	}

return 0;
}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
*/
void tree_skel_destroy(){

	tree_destroy(tree);
	tree = NULL;
	if (pthread_join(processPedidos, NULL) != 0) {
		printf("[ERROR] Couldn't join threads\n");
	}
}

int verify(int op_n){
	//2 - nao conseguiu fazer
	//1 - feita
	//0 - por fazer
	//-1 - nao existe
	struct task_t *atual = malloc(sizeof(struct task_t));
	memcpy(atual, queue_head, sizeof(struct task_t));
	while(atual != NULL){
		if(op_n == atual->op_n){
			if(atual->isDone == 1){
				return 1;
			} else if(atual->isDone == -1){
				return 2;
			} else {
				return 0;
			}
		}
		if(atual->next == NULL)	break;
		atual = atual->next;
	}
	return -1;
}

void *process_task(void *params){
	while (1) { //VER SE ESTE WHILE BAZA

		pthread_mutex_lock(&queue_lock);
		//enquanto ha tarefas por fazer
		while(last_assigned == op_count){
			pthread_cond_wait(&queue_not_empty, &queue_lock);
		}

		struct task_t *task = queue_head;

		for (int i = 0; i < op_count; i++) {
			task = task->next;
		}
		printf("Second thread executing process number %d\n\n", task->op_n);

		pthread_mutex_lock(&tree_lock); //PARA NAO HAVER MAIS NNG A MEXER NA TREE

		if(task->op == 0){ //OP DELETE
			if (tree_del(tree,task->key) == 0) {
				task->isDone = 1;
			}else {
				task->isDone = -1;
			}
		} else { //OP_PUT
			struct data_t *data_put;
			data_put = data_create2(task->datasize, task->data);

			if (tree_put(tree, task->key, data_put) == 0) {
				task->isDone = 1;
			} else {
				task->isDone = -1;
			}
		}

		pthread_mutex_unlock(&tree_lock);
		pthread_mutex_unlock(&queue_lock);
		op_count++;
		printf("Op_count: %d | Last_assigned: %d\n\n", op_count, last_assigned);
		//se o nr de operacoes que tem de fazer for igual ao nr de operacoes feitas

	}
	return NULL;
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
	char **chaves;
	int verifica;
	struct task_t *atual;

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

		pthread_mutex_lock(&queue_lock);
		msg->message->tree_info = last_assigned;

		if (queue_head == NULL){
			msg->message->opcode = 99;
			msg->message->c_type = 60;
			msg->message->data = NULL;
			msg->message->key = NULL;
			msg->message->datatsize = 0;
			return -1;
		}

		atual = queue_head;
		while(atual->next != NULL){
			atual = atual->next;
		}
		atual->next = malloc(sizeof(struct task_t));

		if (atual == NULL || atual->next == NULL){
			msg->message->opcode = 99;
			msg->message->c_type = 60;
			msg->message->data = NULL;
			msg->message->key = NULL;
			msg->message->datatsize = 0;
			return -1;
		}

		//preencher todos os campos da task_t
		atual->op_n = last_assigned;
		atual->op = 0;
		atual->key = strdup(msg->message->key);
		atual->data = NULL;
		atual->datasize = 0;
		atual->isDone = 0;

		last_assigned++;

		pthread_cond_signal(&queue_not_empty);
		pthread_mutex_unlock(&queue_lock);

		if (atual->op_n == last_assigned){
			msg->message->opcode = 99;
			msg->message->c_type = 60;
			msg->message->data = NULL;
			msg->message->key = NULL;
			msg->message->datatsize = 0;
			return -1;
		}

		msg->message->opcode += 1;
		msg->message->c_type = 50;
		msg->message->data = NULL;
		msg->message->key = NULL;
		msg->message->datatsize = 0;

		return 0;
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

		pthread_mutex_lock(&queue_lock);
		msg->message->tree_info = last_assigned;

		if (queue_head == NULL){
			msg->message->opcode = 99;
			msg->message->c_type = 60;
			msg->message->data = NULL;
			msg->message->key = NULL;
			msg->message->datatsize = 0;
			return -1;
		}

		atual = queue_head;
		while(atual->next != NULL){
			atual = atual->next;
		}
		atual->next = malloc(sizeof(struct task_t));

		if (atual == NULL || atual->next == NULL){
			msg->message->opcode = 99;
			msg->message->c_type = 60;
			msg->message->data = NULL;
			msg->message->key = NULL;
			msg->message->datatsize = 0;
			return -1;
		}

		//preencher todos os campos da task_t
		atual->op_n = last_assigned;
		atual->op = 1;
		atual->key = strdup(msg->message->key);
		atual->data = (char *) strdup(msg->message->data);
		atual->datasize = msg->message->datatsize;
		atual->isDone = 0;
		last_assigned++;

		pthread_cond_signal(&queue_not_empty);
		pthread_mutex_unlock(&queue_lock);

		if (atual->op_n == last_assigned){
			msg->message->opcode = 99;
			msg->message->c_type = 60;
			msg->message->data = NULL;
			msg->message->key = NULL;
			msg->message->datatsize = 0;
			return -1;
		}

		msg->message->opcode += 1;
		msg->message->c_type = 50;
		msg->message->data = NULL;
		msg->message->key = NULL;
		msg->message->datatsize = 0;

		return 0;
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

		case 70: //OP_VERIFY
		if ((verifica = verify(msg->message->tree_info)) >= 0) {
			msg->message->opcode += 1;
			msg->message->c_type = 50;
			msg->message->tree_info = verifica;
			return 0;
		} else {
			msg->message->opcode = 99;
			msg->message->c_type = 60;
			return 0;
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
