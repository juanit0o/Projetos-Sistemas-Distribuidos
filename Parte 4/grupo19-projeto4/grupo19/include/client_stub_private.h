//GRUPO 19
//Diogo Pinto - 52763
//Francisco Ramalho - 53472
//João Funenga - 53504

#include <netinet/in.h>

struct rtree_t {
  char *ip;
  char *port;
  struct sockaddr_in sock_addr;
  int sock_desc;

  //zookeeper
  //identificador do seu node
  //identificador do node do outro servidor(primary ou backup)
};
