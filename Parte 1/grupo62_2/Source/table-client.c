//Grupo 62: Pedro Afonso Neves fc46430
//Ricardo Calçado fc46382
//Tiago Ferreira fc47895

/*
	Programa cliente para manipular tabela de hash remota.
	Os comandos introduzido no programa não deverão exceder
	80 carateres.

	Uso: table-client <ip servidor>:<porta servidor>
	Exemplo de uso: ./table_client 10.101.148.144:54321
*/

#include "message.h"
#include "message-private.h"
#include "network_client-private.h"
#include "network_client.h"
#include "table-private.h"
#include "table.h"
#include "data.h"
#include "entry.h"
#include "inet.h"

#include <errno.h>
#include <stdbool.h>
//#include <stdio.h>
//#include <string.h>

void printInvalidArgs() {
	printf("Uso: table-client <ip servidor>:<porta servidor>\n");
	printf("Exemplo de uso: ./table-client 127.0.0.1:12345\n");
}

int opcode(char *command){
  if (strcmp(command,"put") == 0) {
      return OC_PUT;
  } else if (strcmp(command,"get") == 0) {
      return OC_GET;
  } else if (strcmp(command,"update") == 0) {
      return OC_UPDATE;
  } else if (strcmp(command,"size") == 0) {
      return OC_SIZE;
  } else if (strcmp(command,"collisions") == 0) {
      return OC_COLLS;
  } else {
      return OC_RT_ERROR;
  }
}

void print_message(struct message_t *msg) {
    int i;

    printf("\n----- MESSAGE -----\n");
    printf("Tabela número: %d\n", msg->table_num);
    printf("opcode: %d, c_type: %d\n", msg->opcode, msg->c_type);
    switch(msg->c_type) {
        case CT_ENTRY:{
            printf("key: %s\n", msg->content.entry->key);
            printf("datasize: %d\n", msg->content.entry->value->datasize);
        }break;
        case CT_KEY:{
            printf("key: %s\n", msg->content.key);
        }break;
        case CT_KEYS:{
            for(i = 0; msg->content.keys[i] != NULL; i++) {
                printf("key[%d]: %s\n", i, msg->content.keys[i]);
            }
        }break;
        case CT_VALUE:{
            printf("datasize: %d\n", msg->content.data->datasize);
        }break;
        case CT_RESULT:{
            printf("result: %d\n", msg->content.result);
        }break;
        case OC_RT_ERROR:{
            printf("result: %d\n", msg->content.result);
        };
    }
    printf("-------------------\n");
}

int main(int argc, char **argv){
	struct server_t *server;
	char input[81];
	struct message_t *msg_out, *msg_resposta;
	bool quit = false;
	char *command;
	char *input_table;
	char *input_key;
	char *input_data;
	int oc, slen;

	/* Testar os argumentos de entrada */

	if (argc != 2) {
		printInvalidArgs();
		return -1;
	} else if (strstr(argv[1], ":") == NULL) {
		printInvalidArgs();
		return -1;
	}

	/* Usar network_connect para estabelcer ligação ao servidor */
	server = network_connect(argv[1]);

	if (server == NULL) {
		return -1;
	}

	/* Fazer ciclo até que o utilizador resolva fazer "quit" */

	quit = false;

 	while ( ! quit ){

		printf(">>> "); // Mostrar a prompt para inserção de comando

		/* Receber o comando introduzido pelo utilizador
		   Sugestão: usar fgets de stdio.h
		   Quando pressionamos enter para finalizar a entrada no
		   comando fgets, o carater \n é incluido antes do \0.
		   Convém retirar o \n substituindo-o por \0.
		*/

		if (fgets(input, sizeof(input), stdin) == NULL) {
			perror("fgets a NULL");
			quit = true;
		} else {

			slen = strlen(input);
			if (slen > 1) {
				input[slen-1]='\0';
				command = strtok(input, " ");
			} else {
				command = input;
			}

			/* Verificar se o comando foi "quit". Em caso afirmativo
				 não há mais nada a fazer a não ser terminar decentemente.
			 */
			if (strcmp(command,"quit") == 0) {
				quit = true;
			} else {

				/* Caso contrário:

					Verificar qual o comando;

					Preparar msg_out;
					Usar network_send_receive para enviar msg_out para
					o server e receber msg_resposta.
				*/

				msg_out = (struct message_t *) malloc(sizeof(struct message_t));
				msg_resposta = (struct message_t *) malloc(sizeof(struct message_t));

				switch (oc = opcode(command)) {
					case OC_PUT:
						if ( (input_table = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else if ( (input_key = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else if ( (input_data = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else {
							msg_out->opcode = OC_PUT;
							msg_out->c_type = CT_ENTRY;
							msg_out->table_num = atoi(input_table);
							msg_out->content.entry = (struct entry_t *) malloc (sizeof(struct entry_t));
							msg_out->content.entry->key = strdup(input_key);
							msg_out->content.entry->value = data_create2(strlen(input_data),input_data);

							print_message(msg_out);
							msg_resposta = network_send_receive(server, msg_out);
							print_message(msg_resposta);
							if ( (msg_resposta->opcode == OC_PUT + 1) && (msg_resposta->c_type == CT_RESULT) ) {
								printf("Commando processado pelo servidor com sucesso.\n");
							} else {
								printf("Commando não processado pelo servidor.\n");
							}
						}
						free_message(msg_out);
						free_message(msg_resposta);
						break;
					case OC_UPDATE:
						if ( (input_table = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else if ( (input_key = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else if ( (input_data = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else {
							msg_out->opcode = OC_UPDATE;
							msg_out->c_type = CT_ENTRY;
							msg_out->table_num = atoi(input_table);
							msg_out->content.entry = (struct entry_t *) malloc (sizeof(struct entry_t));
							msg_out->content.entry->key = strdup(input_key);
							msg_out->content.entry->value = data_create2(strlen(input_data),input_data);

							print_message(msg_out);
							msg_resposta = network_send_receive(server, msg_out);
							print_message(msg_resposta);
							if ( (msg_resposta->opcode == OC_UPDATE + 1) && (msg_resposta->c_type == CT_RESULT) ) {
								printf("Commando processado pelo servidor com sucesso.\n");
							} else {
								printf("Commando não processado pelo servidor.\n");
							}
						}
						free_message(msg_out);
						free_message(msg_resposta);

						break;
					case OC_GET :
						if ( (input_table = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else if ( (input_key = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else {
							msg_out->opcode = OC_GET;
							msg_out->c_type = CT_KEY;
							msg_out->table_num = atoi(input_table);
							msg_out->content.key = strdup(input_key);

							print_message(msg_out);
							msg_resposta = network_send_receive(server, msg_out);
							print_message(msg_resposta);
							if ( (msg_resposta->opcode == OC_GET + 1) && (msg_resposta->c_type == CT_KEYS || msg_resposta->c_type == CT_VALUE) ) {
								printf("Commando processado pelo servidor com sucesso.\n");
							} else {
								printf("Commando não processado pelo servidor.\n");
							}
						}
						free_message(msg_out);
						free_message(msg_resposta);

						break;
					case OC_SIZE:
						if ( (input_table = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else {
							msg_out->opcode = OC_SIZE;
							msg_out->c_type = CT_NO_DATA;
							msg_out->table_num = atoi(input_table);
							print_message(msg_out);
							msg_resposta = network_send_receive(server, msg_out);
							print_message(msg_resposta);
							if ( (msg_resposta->opcode == OC_SIZE + 1) && (msg_resposta->c_type == CT_RESULT) ) {
								printf("Commando processado pelo servidor com sucesso.\n");
							} else {
								printf("Commando não processado pelo servidor.\n");
							}
						}
						free_message(msg_out);
						free_message(msg_resposta);
						break;
          case OC_COLLS :
						if ( (input_table = strtok(NULL, " ")) == NULL) {
							oc = OC_RT_ERROR;
						} else {
							msg_out->opcode = OC_COLLS;
							msg_out->c_type = CT_NO_DATA;
							msg_out->table_num = atoi(input_table);
							print_message(msg_out);
							msg_resposta = network_send_receive(server, msg_out);
							print_message(msg_resposta);
							if ( (msg_resposta->opcode == OC_COLLS + 1) && (msg_resposta->c_type == CT_RESULT) ) {
								printf("Commando processado pelo servidor com sucesso.\n");
							} else {
								printf("Commando não processado pelo servidor.\n");
							}
						}
						free_message(msg_out);
						free_message(msg_resposta);
						break;
					default :
            oc = OC_RT_ERROR;
						free_message(msg_out);
						free_message(msg_resposta);
            break;
				}
				if (oc == OC_RT_ERROR) {
          printf("Commando entroduzido incorreto. Por favor, uso um dos commandos seguintes:\n");
					printf("   put <table> <key> <data>, get <table> <key>, update <table> <key> <data>,\n");
					printf("   size <table>, collisions <table>, quit\n");
				}
			}
		}
	}

	return network_close(server);
}
