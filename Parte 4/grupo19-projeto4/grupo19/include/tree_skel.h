//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#ifndef _TREE_SKEL_H
#define _TREE_SKEL_H

#include "sdmessage.pb-c.h"
#include "client_stub_private.h"
#include "tree.h"
#include <netinet/in.h>

struct task_t { //provavelmente esta podia ser todas as tarefas que sao para fazer
  int op_n; //o nr da operacao
  int op; //operacao a executar. op=0 se delete, op=1 se put
  char* key; //chave a remover ou a adicionar
  char* data; //dados a adicionar em caso de put. NULL em caso de delete
//adicionar campos para fila do tipo produtor consumidor
  int datasize;
  struct task_t* next; //proxima tarefa a ser realizada
  int isDone; //0 = por fazer || 1 = feita
};

struct rtree_server {
  char *ip;
  char *port;
  struct sockaddr_in sock_addr;
  int sock_desc;
};



/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke().
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init();

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy();

int verify(int op_n);

void *process_task(void *params);

/* Executa uma operação na árvore (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, árvore nao incializada)
*/
int invoke(struct message_t *msg);
//int network_connectAux(struct rtree_t *rtree);

#endif
