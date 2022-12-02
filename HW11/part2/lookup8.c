/*
 * lookup8 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet TCP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	/* for gethostbyaddr() or gethostbyname() */
	struct hostent *host;
	static int first_time = 1;
	int rc;
	char *addr_str;

	if (first_time) {        /* connect to socket ; resource is server name */
		first_time = 0;

		/* Set up destination address. */
		memset(&server, 0, sizeof(server));
		server.sin_family = AF_INET;
		/* Fill in code. */
		host = gethostbyname(resource);
		if(host == NULL) DIE("gethostbyname() fail");
		addr_str = inet_ntoa( *((struct in_addr *)host->h_addr));
		strcat(addr_str, "\0");
		fprintf(stderr, "addr_str: %s\n", addr_str);
		server.sin_addr.s_addr = inet_addr(addr_str);
		// server.sin_addr.s_addr = inet_addr("127.0.0.1");
		server.sin_port = htons(PORT);

		/* Allocate socket.
		 * Fill in code. */
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd == -1) DIE("socket() fail");

		/* Connect to the server.
		 * Fill in code. */
		rc = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
		if(rc == -1) DIE("connect() fail");
	}

	/* write query on socket ; await reply
	 * Fill in code. */
	rc = send(sockfd, (void *)sought, sizeof(Dictrec), 0);
	if(rc == -1) DIE("send() fail");

	rc = recv(sockfd, (void *)sought, sizeof(Dictrec), 0);
	if(rc == -1) DIE("recv() fail");

	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
