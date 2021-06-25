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

#include "../include/message_private.h"
#include "../include/network_server.h"

int sock_desc;

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port) {

	printf("Connected to port %d\n", port);

	if ((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("[ERROR] Couldn't create socket!\n");
		return -1;
	}

	int optval = 1;
	if (setsockopt(sock_desc, SOL_SOCKET, SO_REUSEADDR, (int *) &optval, sizeof(optval)) < 0) {
		printf("[ERROR] Couldn't resuse socket\n");
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;  //IPV4
	server.sin_port = htons(port);  // TCP Port
	server.sin_addr.s_addr = htonl(INADDR_ANY); //ALL MACHINE ADDRESSES

	if(bind(sock_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
		printf("[ERROR] Couldn't bind to socket!\n");
		close(sock_desc);
		return -1;
	}

	//128 corresponde ao backlog, tamanho máximo da fila de coneçoes pendentes
	if(listen(sock_desc, 128) < 0) {
		printf("[ERROR] Couldn't listen to socket\n");
		close(sock_desc);
		return -1;
	}
	return sock_desc;

}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket) {

	if(listening_socket < 0) {
		return -1;
	}

	struct sockaddr_in client_addr;
	socklen_t client_size = sizeof(struct sockaddr);  //TAMANHO DO ADDRESS DO CLIENT
	for (;;) {
		printf("\n");
		int socket = accept(listening_socket, (struct sockaddr *) &client_addr, &client_size);
		if(socket == -1) {
			printf("[ERROR] Failed to connect with client!\n");
			return -1;
		}

		printf("New client connected!\n");

		if(tree_skel_init() == -1) {
			printf("[ERROR] Failed to create the tree!\n");
			return -1;
		}

		for (;;) {
			struct message_t *resposta;
			resposta = malloc(sizeof (struct message_t));
			resposta = network_receive(socket);
			if (resposta != NULL) {
				int invoked = invoke(resposta);
				if (invoked == -1) {
					printf("Client disconnected!\n");
					network_send(socket, resposta);
					free(resposta);
					tree_skel_destroy();
					break;
				}

				int send_to_client = network_send(socket, resposta);
				free(resposta);
				if (send_to_client == -1) {
					printf("[ERROR] Couldn't send message to client\n");
				}
			}
		}
	}
	close(listening_socket);
	return 0;

}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket) {

	if (client_socket < 0) {
		printf("[ERROR] Invalid socket descriptor!\n");
		return NULL;
	}

	struct message_t *rcv_message;
	rcv_message = malloc(sizeof(struct message_t));

	char *buffer;
	buffer = malloc(sizeof(struct message_t) + sizeof(MessageT));
	int result = recv(client_socket, buffer, sizeof(struct message_t) + sizeof(MessageT), 0);

	if (result < 0) {
		free(buffer);
		printf("[ERROR] Invalid read!\n");
		return NULL;
	}

	rcv_message->message = malloc(sizeof (MessageT));
	if (rcv_message->message == NULL) {
		printf("[ERROR] Malloc failed!\n");
		return NULL;
	}
	rcv_message->message = message_t__unpack(NULL, result,(uint8_t *) buffer);

	if (rcv_message->message->opcode != 0) {
		printf("Message received from client\n");
		printf(" OPCODE: \'%d\'\n", rcv_message->message->opcode);
		printf(" DATA: \'%s\'\n", rcv_message->message->data);
		printf(" C-TYPE: \'%d\'\n", rcv_message->message->c_type);
		printf(" KEY: \'%s\'\n", rcv_message->message->key);
		printf(" DATA SIZE: \'%d\'\n", rcv_message->message->datatsize);
		printf("\n");
	}
	free(buffer);
	return rcv_message;

}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg) {

	if (client_socket < 0 || msg == NULL) return -1;

	if (msg->message->opcode != 0) {
		printf("Message sent to client\n");
		printf(" OPCODE: \'%d\'\n", msg->message->opcode);
		printf(" DATA: \'%s\'\n", msg->message->data);
		printf(" C-TYPE: \'%d\'\n", msg->message->c_type);
		printf(" KEY: \'%s\'\n", msg->message->key);
		if (msg->message->keys != NULL) {
			printf(" KEYS: ");
			for (int i = 0; msg->message->keys[i] != NULL; ++i) {
				printf("%s ", msg->message->keys[i]);
			}
			printf("\n");
		}
    		printf(" DATA SIZE: \'%d\'\n", msg->message->datatsize);
    		printf("\n");
  	}

	char *buf;
	int len = message_t__get_packed_size(msg->message);
	buf = malloc(len);

	//serializar message_t
	int tamanho = message_t__pack(msg->message, (uint8_t *) buf);

	//enviar a string para a socket
	int size = send(client_socket, buf, tamanho, 0);
	message_t__free_unpacked(msg->message, NULL);
	free(buf);
	return size;

}

/* A função network_server_close() liberta os recursos alocados por
 * network_server_init().
 */
int network_server_close() {

	return close(sock_desc);

}
