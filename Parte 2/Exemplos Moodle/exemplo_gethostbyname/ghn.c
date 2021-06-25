/* Print the "hostent" information of the host whose name is
 * specified on the command line. */
#include	<stdio.h>
#include	<sys/types.h>
#include	<netdb.h>		/* for struct hostent */
#include	<sys/socket.h>		/* for AF_INET */
#include	<netinet/in.h>		/* for struct in_addr */
#include	<arpa/inet.h>		/* for inet_ntop() */

int main(int argc, char **argv)
{
	struct in_addr 	*addrptr;
	struct hostent 	*hostptr;
	int 		i;
	char		s[1024];

	if (argc < 2) {
		printf("Host not given\n"); return -1;
	}

	if ( (hostptr = gethostbyname(argv[1])) == NULL) {
		printf("gethostbyname error for host: %s\n", argv[1]);
		return -1;
	}
	/* continues in the next page */	
	
	printf("official host name: %s\n", hostptr->h_name);

	/* go through the list of aliases */
	for (i=0 ; (hostptr->h_aliases)[i] != NULL ; i++)
		printf("	alias: %s\n", (hostptr->h_aliases)[i]);

	printf("addr type = %d, addr length = %d\n",
		hostptr->h_addrtype, hostptr->h_length);

	if(hostptr->h_addrtype == AF_INET) {
		/* Print first address in dotted-decimal notation. */
		addrptr=(struct in_addr*)hostptr->h_addr;
		printf("First Internet address: %s\n", 
			inet_ntop(AF_INET,addrptr,s,1024));
		
		/* Print other address in dotted-decimal notation. */
		for (i=1 ; hostptr->h_addr_list[i] != NULL ; i++) {
			addrptr = (struct in_addr*)hostptr->h_addr_list[i];
			printf("Internet address: %s\n", 
				inet_ntop(AF_INET,addrptr,s,1024));
		}
	} else {
		printf("unknown address type\n"); return -1;
	}
}
