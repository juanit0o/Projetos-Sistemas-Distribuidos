
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

int
testInput(int argc)
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

    // Cria socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erro ao criar socket");
        return -1;
    }
	
    // Preenche estrutura server para enviar dados
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server.sin_addr) < 1){
        printf("Erro ao converter IP\n");
        return -1;
    }

    // Envia string
    if((nbytes = sendto(sockfd, str, strlen(str), 0, 
                    (struct sockaddr *) &server, sizeof(server))) != strlen(str)) {
        printf("Erro ao enviar string [%d]: enviados = %d, tamanho string = %d\n", 
                     errno, nbytes, strlen(str));
        close(sockfd);
        return -1;
    }

    printf("'A espera de resposta do servidor ...\n");

    // Espera resposta do servidor
    if ((nbytes = recvfrom(sockfd, &count, sizeof(count), 0, 
                    (struct sockaddr *) 0,  (socklen_t *) 0)) != sizeof(count)) {
        printf("Erro ao receber resposta do servidor: recebidos = %d\n", nbytes);
        return -1;
    } 

    printf("O tamanho da string e' %d bytes!\n", ntohl(count));

    close(sockfd);
    return 0;
}
	
