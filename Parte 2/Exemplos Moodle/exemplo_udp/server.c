/*
 * Programa cliente:
 *   Recebe uma string ao servidor, e envia o tamanho dessa string.
 *
 * Como compilar:
 *   gcc -Wall -g server.c -o server
 *
 * Como executar:
 *   server <porto_servidor>
 */

#include "inet.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in server, client;
    char str[MAX_MSG+1];
    int nbytes, count, size_client;

    // Verifica se foi passado algum argumento
    if (argc != 2){
        printf("Uso: ./server <porto_servidor>\n");
        printf("Exemplo de uso: ./server 12345\n");
        return -1;
    }

    // Cria socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Erro ao criar socket");
        return -1;
    }

    // Preenche estrutura server para bind
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    // Faz bind
    if (bind(sockfd, (struct sockaddr *) &server, sizeof(server))<0){
        perror("Erro ao fazer bind");
        return -1;
    };

    printf("Servidor 'a espera de dados\n");

    while (1) {

        // Recebe string
        size_client = sizeof(client);
        if ((nbytes = recvfrom(sockfd, str, MAX_MSG, 0, 
                   (struct sockaddr *) &client, &size_client)) <= 0) {
            printf("Erro ao receber a string\n");
            return -1;
        }

        // Coloca terminador de string
        str[nbytes] = '\0';

        // Conta numero de caracteres
        count = strlen(str);

        // Converte count para formato de rede
        count = htonl(count);

        // Envia numero de caracteres para o cliente
        if ((nbytes = sendto(sockfd, &count, sizeof(count), 0, 
                       (struct sockaddr *) &client, sizeof(client))) != sizeof(count)) {
            printf("Erro ao enviar o numero de caracteres\n");
            return -1;
        }
    }

/* nunca chega aqui */
    close(sockfd);
    return 0;
}

