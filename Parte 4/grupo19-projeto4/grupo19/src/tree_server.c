//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/message_private.h"
#include "../include/tree_skel.h"
#include "../include/network_server.h"

char *ipPortZoo;
char *port;

int main(int argc, char **argv) {

	if (argc != 3) {
		printf("[ERROR] Invalid input! Try ./tree_server <port> <ipZookeeper:portZookeeper>\n");
		return -1;
	}

	port = malloc(strlen(argv[1]));
	strcpy(port, argv[1]);

	/////port e ip do zookeeper
	ipPortZoo = strdup(argv[2]);

	if (ipPortZoo == NULL || port == NULL) {
		printf("[ERROR] Wrong ip's or ports!\n");
		return NULL;
	}

	//Portas TCP registradas validas
	if (atoi(port) > 49152 || atoi(port) < 1023) {
		printf("[ERROR] Invalid port number! (between 1023 and 49152)\n");
		return -1;
	}

	int socket = network_server_init(atoi(port));

	network_main_loop(socket);

	//QUANDO SAIR DO LOOP, FECHA SERVIDOR
	network_server_close();
	tree_skel_destroy();
	free(port);

}
