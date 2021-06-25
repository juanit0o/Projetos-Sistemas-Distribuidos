//Grupo 62: Pedro Afonso Neves fc46430
//Ricardo Calçado fc46382
//Tiago Ferreira fc47895

/*
   Programa que implementa um servidor de uma tabela hash com chainning.
   Uso: table-server <port> <table1_size> [<table2_size> ...]
   Exemplo de uso: ./table_server 54321 10 15 20 25
*/
#include <error.h>
#include <stdbool.h>

#include "inet.h"
#include "network_client-private.h"
#include "table-private.h"
#include "entry.h"
#include "message-private.h"
#include "message.h"
#include "data.h"


/* Função para preparar uma socket de receção de pedidos de ligação.
*/
int make_server_socket(short port){
  int socket_fd;
  struct sockaddr_in server;

  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("Erro ao criar socket");
    return -1;
  }

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0){
      perror("Erro ao fazer bind");
      close(socket_fd);
      return -1;
  }

  if (listen(socket_fd, 0) < 0){
      perror("Erro ao executar listen");
      close(socket_fd);
      return -1;
  }
  return socket_fd;
}

/* Função que recebe uma tabela e uma mensagem de pedido e:
	- aplica a operação na mensagem de pedido na tabela;
	- devolve uma mensagem de resposta com o resultado.
*/
struct message_t *process_message(struct message_t *msg_pedido, struct table_t **tabela, int lTabela){
	struct message_t *msg_resposta;
  int size, colls_result, result;
  struct data_t *data_result;

	/* Verificar parâmetros de entrada */
  if(msg_pedido == NULL || tabela == NULL){
    return NULL;
  }
	/* Verificar opcode e c_type na mensagem de pedido */
  if( (msg_pedido->opcode < 0 || msg_pedido->opcode > OC_COLLS) ||
      (msg_pedido->c_type < 0 || msg_pedido->opcode > CT_ENTRY) ){
    return NULL;
  }

  /* Aplicar operação na tabela */
  msg_resposta = (struct message_t *) malloc (sizeof(struct message_t));

  int table_num = msg_pedido->table_num - 1;

  /* Validar a tabela sobre o qual o pedido é feito existe */
  if (msg_pedido->table_num <= 0 || msg_pedido->table_num > lTabela) {
    msg_resposta->opcode = OC_RT_ERROR;
    msg_resposta->c_type = CT_RESULT;
    msg_resposta->content.result = ERR_CODE_OUT_OF_RANGE;  // a tabela pedida não existe
    return msg_resposta ;
  }

  switch(msg_pedido->opcode){

    case OC_PUT:
      result = table_put(tabela[table_num],msg_pedido->content.entry->key,msg_pedido->content.entry->value);
      if (result == -1){
        msg_resposta->table_num = table_num + 1;
        msg_resposta->opcode = OC_RT_ERROR;
        msg_resposta->c_type = CT_RESULT;
        msg_resposta->content.result = ERR_CODE_TABLE_FULL;  // Não é possivel guardar o valor
      } else {
        msg_resposta->table_num = table_num + 1;
        msg_resposta->opcode = OC_PUT+1;
        msg_resposta->c_type = CT_RESULT;
      }

      break;
    case OC_UPDATE:
      result = table_update(tabela[table_num],msg_pedido->content.entry->key,msg_pedido->content.entry->value);
      if (result == -1){
        msg_resposta->table_num = table_num + 1;
        msg_resposta->opcode = OC_RT_ERROR;
        msg_resposta->c_type = CT_RESULT;
        msg_resposta->content.result = ERR_CODE_NO_UPDATE;  // Não é possivel atualizar o valor
      } else {
        msg_resposta->table_num = table_num + 1;
        msg_resposta->opcode = OC_UPDATE+1;
        msg_resposta->c_type = CT_RESULT;
      }

      break;

   case OC_GET:
    //função na tabela get
    if (strcmp(msg_pedido->content.key,"*") == 0) {
      msg_resposta->content.keys = table_get_keys(tabela[table_num]);
      msg_resposta->opcode = OC_GET + 1;
      msg_resposta->c_type = CT_KEYS;
    } else {
      data_result = table_get(tabela[table_num], msg_pedido->content.key);

      //verificação do get
      if(data_result == NULL){
        msg_resposta->content.data = data_create(0);
      } else {
        msg_resposta->content.data = data_dup(data_result);
      }
      msg_resposta->table_num = table_num + 1;
      msg_resposta->opcode = OC_GET + 1;
      msg_resposta->c_type = CT_VALUE;
    }
    break;

  case OC_SIZE:
      size = getSize(tabela[table_num]);
      if(size < 0){
        msg_resposta->table_num = table_num + 1;
        msg_resposta->opcode = OC_RT_ERROR;
        msg_resposta->c_type = CT_RESULT;
        msg_resposta->content.result = ERR_CODE;
      } else {
        msg_resposta->table_num=table_num + 1;
        msg_resposta->opcode = OC_SIZE+1;
        msg_resposta->c_type = CT_RESULT;
        msg_resposta->content.result = size;
      }
      break;

 case OC_COLLS:
      //função na tabela collisions
      colls_result = getCollisions(tabela[table_num]);
      if(colls_result < 0){
        msg_resposta->table_num = table_num + 1;
        msg_resposta->opcode = OC_RT_ERROR;
        msg_resposta->c_type = CT_RESULT;
        msg_resposta->content.result = ERR_CODE;
      } else {
        msg_resposta->table_num=table_num + 1;
        msg_resposta->opcode = OC_COLLS+1;
        msg_resposta->c_type = CT_RESULT;
        msg_resposta->content.result = colls_result;
      }
      break;
}
	/* Preparar mensagem de resposta */
	return msg_resposta;
}


/* Função "inversa" da função network_send_receive usada no table-client.
   Neste caso a função implementa um ciclo receive/send:

	Recebe um pedido;
	Aplica o pedido na tabela;
	Envia a resposta.
*/
int network_receive_send(int sockfd, struct table_t **tables, int lTabela){
  char *message_resposta, *message_pedido;
  int msg_length;
  int message_size; //message_resposta
  int msg_size; //message_pedido
  int result;
  struct message_t *msg_pedido, *msg_resposta;

	/* Verificar parâmetros de entrada */
  if(sockfd < 0 || tables == NULL){
    return -1;
  }

	/* Com a função read_all, receber num inteiro o tamanho da
	   mensagem de pedido que será recebida de seguida.*/
	result = read_all(sockfd, (char *) &msg_size, _INT);

	/* Verificar se a receção teve sucesso */
  if(result < 0){
    return -1;
  }
	/* Alocar memória para receber o número de bytes da
	   mensagem de pedido. */
  msg_length = ntohl(msg_size);
  message_pedido = (char *) malloc(sizeof(char) * msg_length);

	/* Com a função read_all, receber a mensagem de resposta. */
	result = read_all(sockfd, message_pedido, msg_length);

	/* Verificar se a receção teve sucesso */
  if(result < 0){
    return -1;
  }
	/* Desserializar a mensagem do pedido */
	msg_pedido = buffer_to_message(message_pedido, msg_length);

	/* Verificar se a desserialização teve sucesso */
  if(msg_pedido == NULL){
    return -1;
  }

	/* Processar a mensagem */

	msg_resposta = process_message(msg_pedido, tables,lTabela);

	/* Serializar a mensagem recebida */
	message_size = message_to_buffer(msg_resposta, &message_resposta);

	/* Verificar se a serialização teve sucesso */
  if(message_size < 0){
    return -1;
  }
	/* Enviar ao cliente o tamanho da mensagem que será enviada
	   logo de seguida
	*/
	msg_size = htonl(message_size);
 	result = write_all(sockfd, (char *) &msg_size, _INT);

	/* Verificar se o envio teve sucesso */
  if (result < 0){
    return -1;
  }
	/* Enviar a mensagem que foi previamente serializada */
	result = write_all(sockfd, message_resposta, message_size);

	/* Verificar se o envio teve sucesso */
  if(result < 0){
    return -1;
  }
	/* Libertar memória */
  free(message_resposta);
	return 0;
}



int main(int argc, char **argv){
	int listening_socket, connsock; //, result;
  int tamanho_tabela;
	struct sockaddr_in client;
	socklen_t size_client;
	struct table_t **tables;
  bool clientIsConnected;

	if (argc < 3){
    printf("Uso: ./server <porta TCP> <port> <table1_size> [<table2_size> ...]\n");
    printf("Exemplo de uso: ./table-server 54321 10 15 20 25\n");
    return -1;
	}

	if ((listening_socket = make_server_socket(atoi(argv[1]))) < 0){
   return -1;
  }

	/*********************************************************/
	/* Criar as tabelas de acordo com linha de comandos dada */
	/*********************************************************/

  tables = (struct table_t **) malloc(sizeof(struct table_t *) * (argc-1));

  for(tamanho_tabela=0; tamanho_tabela<argc-2 ; tamanho_tabela++) {
    if ( (tables[tamanho_tabela] = table_create(atoi(argv[tamanho_tabela+2]))) == NULL ) {
      printf("An error occurs in the creation of the tables. Server is closing ...\n");
      close(listening_socket);
      return -1;
    } else {
      printf("table[%i] criada com dimensão: %s\n",tamanho_tabela,argv[tamanho_tabela+2]);
    }

  }

	while ((connsock = accept(listening_socket, (struct sockaddr *) &client, &size_client)) != -1) {
		printf(" * Client is connected!\n");
    clientIsConnected = true;

		while ( clientIsConnected ){

			/* Fazer ciclo de pedido e resposta */
			if ( network_receive_send(connsock, tables, tamanho_tabela) < 0) {
        clientIsConnected = false;
      }

			/* Ciclo feito com sucesso ? Houve erro?
			   Cliente desligou? */

		}
    printf(" * Client is disconnected!\n");
    close(connsock);
	}

  printf("Server closing ...\n");
  close(listening_socket);
  return 0;
}
