GRUPO 19

Diogo Pinto - 52763
Francisco Ramalho - 53472
João Funenga - 53504

------------------- Para compilar / testar --------------------
Para compilar apenas é necessário executar o comando 'make' num 
terminal na diretoria root (serao gerados tanto a aplicacao 
executavel do cliente (tree_client) como a do servidor 
(tree_server) na pasta bin/).

Para usar a aplicacao, primeiro é necessário iniciar o serviço 
zookeeper, que vai estar sempre a correr em background.

Para criar um servidor é necessario fazer num terminal: 
bin/tree_server <NrPorto>  <IpZookeeper>:<NrPortoZookeeper>.
(Testamos com o NrPorto 8081 para o primario e com o NrPorto 
8082 para o backup)
Para o IpZookeeper:NrPortoZookeeper, usamos 127.0.0.1:2181.

Para cada cliente, é necessário outro terminal na mesma diretoria,
correr o cliente com: bin/tree_client <IpZookeeper:PortoZookeeper> 
(Testamos com Ip = 127.0.0.1 e Porto igual ao do servidor (2181), 
onde está o servidor Zookeeper)

Depois de conectados, o cliente pode fazer pedidos ao servidor, 
cuja sintaxe para cada comando  pode ser obtida atraves do comando 'help'.

Os pedidos de escrita ('put' e 'del') recebem resposta do servidor
com o id da operação. Para verificar se a operação x foi replicada para o 
servidor de backup, deve-se fazer 'verify x'. Para todas as outras 
operações a resposta é dada instantaneamente (sincronamente).

Quando se pretende desconectar, o cliente pode escrever quit e sera 
automaticamente desconectado do servidor a que estava conectado previamente.

Quando apenas está a correr um dos servidores, não são aceites comandos.

-------------------- Comando de execução ----------------------

make - Compilar ambos os ficheiros do servidor e cliente e gerar os executaveis respetivos

make tree_client - Compilar os ficheiros relativos ao client e criar o executavel deste

make tree_server - Compilar os ficheiros relativos ao server e criar o executavel deste

make clean - Apaga os ficheiros dentro das diretorias /bin, /obj, /lib e o .h e .c do protobuf

valgring --leak-check=full ./bin/{file_name} - Verificar os leaks 
                                               de memória em cada 
					       ficheiro

----------------------- Informação extra ----------------------

Código testado e compilado na imagem de linux fornecida pelo DI
