/*
 * Programa cliente:
 *   Envia uma string ao servidor, e recebe o tamanho dessa string.
 *
 * Como compilar:
 *   gcc -Wall -g client.c -o client
 * 
 * Como executar:
 *   ./client <ip_servidor> <porto_servidor> <string>
 */

#include "inet.h"
#include <errno.h>

int testInput(int argc)
{
    if (argc != 4){
        printf("Uso: ./client <ip_servidor> <porto_servidor> <string>\n");
        printf("Exemplo de uso: ./client 127.0.0.1 12345 olacomovais\n");
        return -1;
    } 
    
    return 0;
}

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_in server;
    char str[MAX_MSG];
    int count, nbytes;

    // Verifica se foi passado algum argumento
    if (testInput(argc) < 0) return -1;
    
    // Copia os primeiros MAX_MSG-1 bytes da string passada como argumento
    strncpy(str, argv[3], MAX_MSG-1);

    // Garante que a string tem terminacao
    str[MAX_MSG-1] = '\0';

    // Cria socket TCP
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar socket TCP");
        return -1;
    }
	
    // Preenche estrutura server para estabelecer conexão
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server.sin_addr) < 1) {
        printf("Erro ao converter IP\n");
        close(sockfd);
        return -1;
    }

    // Estabelece conexão com o servidor definido em server
    if (connect(sockfd,(struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Erro ao conectar-se ao servidor");
        close(sockfd);
        return -1;
    }

    // Envia string
    if((nbytes = write(sockfd,str,strlen(str))) != strlen(str)){
        perror("Erro ao enviar dados ao servidor");
        close(sockfd);
        return -1;
    }

    printf("'A espera de resposta do servidor ...\n");

    // Recebe tamanho da string
    if((nbytes = read(sockfd,&count,sizeof(count))) != sizeof(count)){
        perror("Erro ao receber dados do servidor");
        close(sockfd);
        return -1;
    };

    printf("O tamanho da string e' %d!\n", ntohl(count));

    // Fecha o socket
    close(sockfd);
    return 0;
}

