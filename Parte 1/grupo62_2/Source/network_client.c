//Grupo 62: Pedro Afonso Neves fc46430
//Ricardo Calçado fc46382
//Tiago Ferreira fc47895

#include "message.h"
#include "message-private.h"
#include "network_client-private.h"
#include "network_client.h"
#include "inet.h"

#include <stdlib.h>


int write_all(int sock, char *buf, int len){
	int size = len;
  while(size>0) {
      int res = write(sock, buf, size);
      if(res<0) {
          perror("A escrita falhou:");
          return res;
      }
      buf += res;
      size -= res;
  }
  return len;
}


int read_all(int sock, char *buf, int len){
	int size = len;
  while(size>0) {
      int res = read(sock, buf, size);
      if(res == 0)
          return 0;
      if(res<0) {
          perror("A leitura falhou:");
          return res;
      }
      buf += res;
      size -= res;
  }
  return len;
}


struct server_t *network_connect(const char *address_port){

	/* Verificar parâmetro da função e alocação de memória */
	if(address_port == NULL){
		return NULL;
	}

	char *cAPort = strdup(address_port);

	if(cAPort == NULL){
		return NULL;
	}

	char *serverIP = strtok(cAPort,":");
	char *porto = strtok(NULL,"\0");

	struct server_t *server = malloc(sizeof(struct server_t));

	if(server == NULL) {
		free(cAPort);
		return NULL;
  }
	/* Estabelecer ligação ao servidor:

		Preencher estrutura struct sockaddr_in com dados do
		endereço do servidor.

		Criar a socket.

		Estabelecer ligação.
	*/

	/* Se a ligação não foi estabelecida, retornar NULL */

	if ((server->sockServer = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro ao criar socket TCP");
		free(server);
		free(cAPort);
		return NULL;
	}

	server->serverIn.sin_family = AF_INET; // família de endereços
	server->serverIn.sin_port = htons(atoi(porto)); // Porta TCP
	if (inet_pton(AF_INET, serverIP, &server->serverIn.sin_addr) < 1) { // Endereço IP
		printf("Erro ao converter IP\n");
		free(cAPort);
		close(server->sockServer);
		free(server);
		return NULL;
	}

	if (connect(server->sockServer,(struct sockaddr *)&server->serverIn, sizeof(server->serverIn)) < 0) {
		perror("Erro ao conectar-se ao servidor");
		free(cAPort);
		close(server->sockServer);
		free(server);
		return NULL;
	}

	free(cAPort);

	return server;
}

struct message_t *network_send_receive(struct server_t *server, struct message_t *msg){
	char *message_out;
	int message_size, msg_size, result;
	struct message_t *msg_resposta;

	/* Verificar parâmetros de entrada */

	if(server == NULL || msg == NULL){
		return NULL;
	}

	/* Serializar a mensagem recebida */
	message_size = message_to_buffer(msg, &message_out);

	/* Verificar se a serialização teve sucesso */

	if(message_size == -1){
		return NULL;
	}

	/* Enviar ao servidor o tamanho da mensagem que será enviada
	   logo de seguida
	*/
	msg_size = htonl(message_size);
 	result = write_all(server->sockServer, (char *) &msg_size, _INT);

	/* Verificar se o envio teve sucesso */

	if(result != _INT){
		free(message_out);
		close(server->sockServer);
		return NULL;
	}

	/* Enviar a mensagem que foi previamente serializada */

	result = write_all(server->sockServer, message_out, message_size);

	/* Verificar se o envio teve sucesso */

	if(result != message_size) {
		free(message_out);
		close(server->sockServer);
		return NULL;
	}

	/* De seguida vamos receber a resposta do servidor:

		Com a função read_all, receber num inteiro o tamanho da
		mensagem de resposta.

		Alocar memória para receber o número de bytes da
		mensagem de resposta.

		Com a função read_all, receber a mensagem de resposta.

	*/

	printf("Aguardando resposta do servidor ...\n");

	result = read_all(server->sockServer, (char *) &msg_size,_INT);

	if(result != _INT){
		free(message_out);
		close(server->sockServer);
    return NULL;
	}

	message_size = ntohl(msg_size);
	char *msg_aux = malloc(sizeof(char) * message_size);

	if((result = read_all(server->sockServer, msg_aux, message_size)) != message_size){
		close(server->sockServer);
		return NULL;
	}

	/* Desserializar a mensagem de resposta */
	msg_resposta = buffer_to_message(msg_aux, message_size);

	/* Verificar se a desserialização teve sucesso */

	if(msg_resposta == NULL){
		return NULL;
	}

	/* Libertar memória */

	free(message_out);
	free(msg_aux);
	return msg_resposta;
}

int network_close(struct server_t *server){
	/* Verificar parâmetros de entrada */

	if(server == NULL){
		return -1;
	}

	/* Terminar ligação ao servidor */

	if (close(server->sockServer) < 0){ // fechar a ligacao do cliente para o servidor
    return -1;
	}

	/* Libertar memória */

	free(server);
	return 0;
}
