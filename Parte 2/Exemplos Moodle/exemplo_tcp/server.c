/*
 * Programa servidor:
 *   Recebe uma string do cliente e envia o tamanho dessa string.
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
    int sockfd, connsockfd;
    struct sockaddr_in server, client;
    char str[MAX_MSG+1];
    int nbytes, count, size_client;

    // Verifica se foi passado algum argumento
    if (argc != 2){
        printf("Uso: ./server <porto_servidor>\n");
        printf("Exemplo de uso: ./server 12345\n");
        return -1;
    }

    // Cria socket TCP
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("Erro ao criar socket");
        return -1;
    }

    // Preenche estrutura server para bind
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    // Faz bind
    if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0){
        perror("Erro ao fazer bind");
        close(sockfd);
        return -1;
    };

    // Faz listen
    if (listen(sockfd, 0) < 0){
        perror("Erro ao executar listen");
        close(sockfd);
        return -1;
    };

    printf("Servidor 'a espera de dados\n");

    // Bloqueia a espera de pedidos de conexão
    while ((connsockfd = accept(sockfd,(struct sockaddr *) &client, &size_client)) != -1) {

        // Lê string enviada pelo cliente do socket referente a conexão
        if((nbytes = read(connsockfd,str,MAX_MSG)) < 0){
            perror("Erro ao receber dados do cliente");
            close(connsockfd);
            continue;
        }

        // Coloca terminador de string
        str[nbytes] = '\0';

        // Conta numero de caracteres
        count = strlen(str);

        // Converte count para formato de rede
        count = htonl(count);

        // Envia tamanho da string ao cliente através do socket referente a conexão
        if((nbytes = write(connsockfd,&count,sizeof(count))) != sizeof(count)){
            perror("Erro ao enviar resposta ao cliente");
            close(connsockfd);
            continue;
        }

        // Fecha socket referente a esta conexão
        close(connsockfd);
    }

    // Fecha socket (só executado em caso de erro...)
    close(sockfd);
    return 0;
}

