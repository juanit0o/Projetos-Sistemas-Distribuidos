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
#include <poll.h>

#include "../include/message_private.h"
#include "../include/network_server.h"
#include "../include/tree_skel.h"
#include "../include/network_client.h" //testar e ve se da sem

#define MAX_NUM_CLIENTS 10

int sock_desc;

/* Função para preparar uma socket de receção de pedidos de ligação
* num determinado porto.
* Retornar descritor do socket (OK) ou -1 (erro).
*/
int network_server_init(short port) {

	printf("Connected to port '%d'\n", port);

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

	if(tree_skel_init() == -1) {
		printf("[ERROR] Failed to create the tree!\n");
		return -1;
	}

	struct sockaddr_in client_addr;
	socklen_t client_size = sizeof(struct sockaddr);  //TAMANHO DO ADDRESS DO CLIENT

	struct pollfd connections[MAX_NUM_CLIENTS];

	for(int i = 0; i< MAX_NUM_CLIENTS; i++){
		connections[i].fd = -1;
	}

	connections[0].fd = listening_socket; //ver se ha eventos na welcoming socket
	connections[0].events = POLLIN; //esperar ligacoes nesta socket

	int nfds = 1; //nr de descriptors

	int kfds;
	while((kfds = poll(connections, nfds, -1)) >= 0) { //kfds == 0 eh acontecer timeout e n haver eventos
		if(kfds > 0){					//nr de descritores com eventos OU erro
			if((connections[0].revents & POLLIN) && (nfds < MAX_NUM_CLIENTS)){ //verifica se a listening socket tem um novo pedido de conexao
				printf("\n");
				if((connections[nfds].fd = accept(connections[0].fd, (struct sockaddr *) &client_addr, &client_size)) > 0){
					connections[nfds].events = POLLIN;
					nfds++;
				}
			}

			for (int i = 1; i < nfds; i++) {
				if(connections[i].fd > 0) {

					if(connections[i].revents & POLLIN) { //se tem dados para ler
						struct message_t *resposta;
						resposta = malloc(sizeof (struct message_t));
						resposta = network_receive(connections[i].fd);
						if (resposta != NULL) {
							int invoked = invoke(resposta);
							if (invoked == -1) {
								network_send(connections[i].fd, resposta);
								close(connections[i].fd);
								connections[i].fd = -1;
								for (int j = i; j < MAX_NUM_CLIENTS-1; j++) {
									connections[j] = connections[j+1];
								}
								close(connections[MAX_NUM_CLIENTS-1].fd);
								connections[MAX_NUM_CLIENTS-1].fd = -1;
								nfds--;
								free(resposta);
							}else{

							int send_to_client = network_send(connections[i].fd, resposta);
							free(resposta);
							if (send_to_client == -1) {
								printf("[ERROR] Couldn't send message to client\n");
							}
						}
						}else{
							close(connections[i].fd);
							connections[i].fd = -1; //sera que temos de mudar o evento da conexao fucked?
							for (int j = i; j < MAX_NUM_CLIENTS-1; j++) {
								connections[j] = connections[j+1];
							}
							close(connections[MAX_NUM_CLIENTS-1].fd);
							connections[MAX_NUM_CLIENTS-1].fd = -1;
							nfds--;
						}
					}
					if((connections[i].revents & POLLERR) || (connections[i].revents & POLLHUP)){ //se o descritor tiver erros OU se o descritor foi hanged up (fechou a ligacao)
						close(connections[i].fd);
						connections[i].fd = -1;
						for (int j = i; j < MAX_NUM_CLIENTS-1; j++) {
							connections[j] = connections[j+1];
						}
						close(connections[MAX_NUM_CLIENTS-1].fd);
						connections[MAX_NUM_CLIENTS-1].fd = -1;
						nfds--;
					}
				}
			}
			printf("Number of clients connected: %d\n\n", nfds-1);
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
		printf("  OPCODE: \'%d\'\n  DATA: \'%s\'\n  C-TYPE: \'%d\'\n  KEY: \'%s\'\n  DATA SIZE: \'%d\'\n  TREE INFO: \'%d\'\n\n",
			rcv_message->message->opcode,
			rcv_message->message->data,
			rcv_message->message->c_type,
			rcv_message->message->key,
			rcv_message->message->datatsize,
			rcv_message->message->tree_info);
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
 //se eu for o backup, nao quero responder ao primario MAS TENHO DE RESPONDER AO CLIENTE, so posso fazer network sends de pedidos de leitura

	if (client_socket < 0 || msg == NULL) return -1;

	if (msg->message->opcode != 0) {
		printf("Message sent to client\n");
		printf("  OPCODE: \'%d\'\n  DATA: \'%s\'\n  C-TYPE: \'%d\'\n  KEY: \'%s\'\n  DATA SIZE: \'%d\'\n  TREE INFO: \'%d\'",
			msg->message->opcode,
			msg->message->data,
			msg->message->c_type,
			msg->message->key,
			msg->message->datatsize,
			msg->message->tree_info);
		if (msg->message->keys != NULL) {
			printf("\n  KEYS: ");
			for (int i = 0; msg->message->keys[i] != NULL; ++i) {
				printf("%s ", msg->message->keys[i]);
			}
		}
		printf("\n\n");
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
