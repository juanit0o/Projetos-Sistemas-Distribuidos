//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> //CLOSE DA SOCKET
#include <arpa/inet.h> //inet_pton

#include "../include/client_stub.h"
#include "../include/client_stub_private.h"
#include "../include/network_client.h"
#include "../include/message_private.h"

struct rtree_t;

/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) a base da
 *   informação guardada na estrutura rtree;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtree;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtree_t *rtree) {

	if ((rtree->sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

	rtree->sock_addr.sin_family = AF_INET;
	//Port é inicialmente uma string (split inicial do stdin), logo atoi
	rtree->sock_addr.sin_port = htons(atoi(rtree->port));

	if ((inet_pton(AF_INET, rtree->ip, &rtree->sock_addr.sin_addr)) != 1) {
		printf("[ERROR] Ipv4 to in_addr failed!\n");
		close(rtree->sock_desc);
		return -1;
	}

	if (connect(rtree->sock_desc, (struct sockaddr *) &rtree->sock_addr, sizeof(rtree->sock_addr)) < 0) {
		close(rtree->sock_desc);
		return -1;
	}

	return 0;

}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtree_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
struct message_t *network_send_receive(struct rtree_t * rtree, struct message_t *msg) {

	if(rtree == NULL || msg == NULL) return NULL;

	uint8_t *msg_pedido = NULL;
	char *msg_resposta;
	msg_resposta = malloc(sizeof(struct message_t) + sizeof(MessageT));
	struct message_t *msg_resultado;

	int tamanho = message_t__get_packed_size(msg->message);
	if (tamanho == -1) return NULL;

	int result;
	msg_pedido = malloc(tamanho);
	message_t__pack(msg->message, msg_pedido);
	result = send(rtree->sock_desc, msg_pedido, tamanho, 0);
	if (result < 0) return NULL;
	free(msg_pedido);

	//ESPERAR A MENSAGEM DE RESPOSTA

	tamanho = recv(rtree->sock_desc, msg_resposta, sizeof(struct message_t) + sizeof(MessageT), 0);
	if (tamanho < 0) {
		free(msg_resposta);
		printf("[ERROR] Couldn't read answer from socket \n");
		return NULL;
	}

	msg_resultado = malloc(tamanho);
	msg_resultado->message = malloc(tamanho);
	msg_resultado->message = message_t__unpack(NULL, tamanho, (uint8_t *) msg_resposta);
	if (msg_resultado->message->opcode == 0) {
		printf("Disconnected from server... Try again later\n");
		exit(-1);
		return NULL;
	}
	free(msg_resposta);
	return msg_resultado;

}

/* A função network_close() fecha a ligação estabelecida por
 * network_connect().
 */
int network_close(struct rtree_t * rtree){

	int output = close(rtree->sock_desc);
	free(rtree);
	return output;

}
