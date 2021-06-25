//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/data.h"
#include "../include/entry.h"
#include "../include/client_stub.h"

int main(int argc, char **argv) {

	if (argc != 2) {
		printf("[ERROR] Invalid input! Try ./tree_client <ip>:<port>\n");
		return -1;
	}

	char *ip_port = malloc(strlen(argv[1]));
	strcpy(ip_port, argv[1]);
	printf("Connecting to %s ...\n", ip_port);

	//CONNECT TO REMOTE TREE
	struct rtree_t *rtree;
	rtree = rtree_connect(ip_port);

	if (rtree == NULL) {
		printf("[ERROR] Couldn't connect to the server. Please try again!\n");
		return -1;
	}
	printf("Connected to %s!\n", ip_port);

	char **keys;

	char message_client[1000];
	for (;;) {
		printf("> ");
		char* gets;
		gets = fgets(message_client, 1000, stdin);
		if (gets == NULL) break;
		message_client[strcspn(message_client, "\n")] = '\0';
		int contador = 1;
		for (int i = 0; message_client[i] != '\0'; i++) {
			if (message_client[i] != ' ') {
				contador = 0;
			}
		}
		if (strlen(message_client) <= 0 || message_client == NULL || contador == 1) {
			printf("[ERROR] Invalid input! Type \'help\' for more info\n");
		}else{
			//caso do put
			if (strcmp(strtok(strdup(message_client), " "), "put") == 0) {
				char *msg_dup = strdup(message_client);
				int counter = 0;
				int i;
				for(i = 0; msg_dup[i] != 0; ++i) {
					if(msg_dup[i] == ' '){
						++counter;
					}
				}

			if ((msg_dup[5] == ' ' && msg_dup[6] == 0)
				|| (msg_dup[4] == 'a' && msg_dup[5] == 0) || counter < 2) {
				printf("[ERROR] Invalid put input! Try \'put <key> <data>\'\n");
			}else {
				//PARAMETROS
				char *key = strdup(strtok(NULL, " "));
				char *data = strdup(strtok(NULL, ""));

				if (!key || !data) {
					printf("[ERROR] Invalid put input! Try \'put <key> <data>\'\n");
					break;
				}

				struct data_t *data_put = data_create2(strlen(data), data);
				if(data_put == NULL) printf("[INTERNAL ERROR] Couldn't create data_t!\n");

				struct entry_t *entry_put = entry_create(key, data_put);
				if(entry_put == NULL) printf("[INTERNAL ERROR] Couldn't create entry_t!\n");

				if(rtree_put(rtree, entry_put) != 0) {
					printf("[ERROR] Couldn't add 'put' operation to the queue\n");
				}
					entry_destroy(entry_put);
				}


			} else if (strcmp(strtok(strdup(message_client), " "), "get") == 0) {

				char *msg_dup = strdup(message_client);

				int counter = 0;
				int i;
				for(i = 0; msg_dup[i] != 0; ++i) {
					if(msg_dup[i] == ' '){
						++counter;
					}
				}

				if (counter != 1) {
					printf("[ERROR] Invalid get input! Try \'get <key>\'\n");
        			} else {
					char *key = NULL;

					if (!(key = strdup(strtok(NULL, " ")))) {
						printf("[ERROR] Invalid get input! Try \'get <key>\'\n");
						break;
					}

					struct data_t *data_result = rtree_get(rtree, key);

					if (data_result != NULL) {
						printf("Success! Got element from tree with key '%s': '%s'!\n", key, (char *) data_result->data);
					} else {
						printf("Couldn't get element from tree with key '%s'!\n", key);
					}
					data_destroy(data_result);
				}

			} else if (strcmp(strtok(strdup(message_client), " "), "del") == 0) {

				char *msg_dup;
				msg_dup = malloc(sizeof(char *));
				msg_dup = strdup(message_client);

				int counter = 0;
				for (int i = 0; msg_dup[i] != 0; ++i) {
					if (msg_dup[i] == ' '){
						++counter;
					}
				}

				if (counter != 1 || (msg_dup[3] == ' ' && msg_dup[4] == 0)) {
					printf("[ERROR] Invalid del input! Try \'del <key>\'\n");
				} else {
					char *key = NULL;

					if (!(key = strdup(strtok(NULL, " ")))) {
						printf("[ERROR] Invalid del input! Try \'del <key>\'\n");
						break;
					}

					if (rtree_del(rtree, key) != 0) {
						printf("[ERROR] Couldn't add 'del' operation to the queue\n");
					}

					free(msg_dup);
				}

			} else if (strcmp(strtok(strdup(message_client), " "), "size") == 0) {

				int size = rtree_size(rtree);
				if (size < 0) {
					printf("[ERROR] Couldn't get size of tree!\n");
				} else {
					printf("Current tree size: %d\n", size);
				}

			} else if (strcmp(strtok(strdup(message_client), " "), "height") == 0) {

				int height = rtree_height(rtree);
				if (height < 0) {
					printf("[ERROR] Couldn't get height of tree!\n");
				} else {
					printf("Current tree height: %d\n", height);
				}

			} else if (strcmp(strtok(strdup(message_client), " "), "getkeys") == 0) {
				keys = rtree_get_keys(rtree);

				if (keys != NULL && keys[0] != NULL) {
					for (int i = 0; keys[i] != NULL; ++i) {
						printf("\'%s\' ", keys[i]);
					}
					printf("\n");
					rtree_free_keys(keys);
				} else {
					printf("No keys available!\n");
				}
				}else if (strcmp(strtok(strdup(message_client), " "), "verify") == 0) {

					char *msg_dup = strdup(message_client);

					int counter = 0;
					int i;
					for(i = 0; msg_dup[i] != 0; ++i) {
						if(msg_dup[i] == ' '){
							++counter;
						}
					}

					if (counter != 1) {
						printf("[ERROR] Invalid verify input! Try \'verify <op_n>\'\n");
					} else {
						char *op_n;
						if ((!(op_n = strdup(strtok(NULL, " "))) && (atoi(op_n)%10 != 0)) ||
								(msg_dup[6] == ' ' && msg_dup[7] == '\0')) { //ver se e numero de op valido
							printf("[ERROR] Invalid verify input! Try \'verify <op_n>\'\n");
						}else{

						int verifica = rtree_verify(rtree, atoi(op_n));

						if (verifica == 2) {
							printf("[ERROR] Operation with op_num '%d' couldn't be executed!\n", atoi(op_n));
						} else if (verifica == 1) {	//FEITO
							printf("Success! Operation with op_num '%d' was already done!\n", atoi(op_n));
						} else if (verifica == 0) {	//POR FAZER
							printf("Operation with op_num '%d' hasn't been done yet!\n", atoi(op_n));
						} else {	//NAO EXISTE
							printf("Operation with op_num '%d' doesn't exist!\n", atoi(op_n));
						}
					}
					free(op_n);
					}


			} else if (strcmp(strtok(strdup(message_client), " "), "quit") == 0) {
				printf("Disconnecting...\n");
				break;

			} else if (strcmp(strtok(strdup(message_client), " "), "help") == 0) {
				printf("Possible inputs:\n");
				printf(" put <key> <data>\n");
				printf(" get <key>\n");
				printf(" del <key>\n");
				printf(" size\n");
				printf(" height\n");
				printf(" getkeys\n");
				printf(" verify <op_n>\n");
				printf(" quit\n");
			} else {
				printf("Invalid input! Please type \'help\' to more information!\n");
			}
		}
	}
	rtree_disconnect(rtree);

}
