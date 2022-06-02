#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "env.h"

void write_file(int sockfd, FILE *fp);
ssize_t total = 0;

int main(void) {
	int ret, connfd;
	char addr[INET_ADDRSTRLEN], filename[BUFFSIZE] = {0};
	FILE *fp;
	socklen_t addrlen;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Can't allocate sockfd");
		exit(1);
	}

	struct sockaddr_in clientaddr, serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);

	ret = bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
	if (ret < 0) {
		perror("bind error");
		exit(1);
	}

	ret = listen(sockfd, LISTEN_PORT);
	if (ret < 0) {
		perror("listen error");
		exit(1);
	}

	addrlen = sizeof(clientaddr);
	connfd = accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);
	if (connfd < 0) {
		perror("Connect error");
		exit(1);
	}
	close(sockfd);

	ret = recv(connfd, filename, BUFFSIZE, 0);
	if (ret < 0) {
		perror("Can't receive file name");
		exit(1);
	}

	fp = fopen(filename, "wb");
	if (fp == NULL) {
		perror("cant open file");
		exit(1);
	}

	printf("Start rev %s from %s\n", filename, inet_ntop(AF_INET,
		&clientaddr.sin_addr, addr, INET_ADDRSTRLEN));
	write_file(connfd, fp);
	printf("receive success, numbytes = %ld\n", total);

	fclose(fp);
	close(connfd);
	return 0;
} 


void write_file(int sockfd, FILE *fp)
{
	ssize_t n;
	char buff[MAX_LINE] = {0};
	while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0) {
		total += n;
		if (n == -1) {
			perror("receive file error");
			exit(1);
		}

		if((ssize_t)fwrite(buff, sizeof(char), n, fp) != n) {
			perror("Write file error");
			exit(1);
		}
		memset(buff, 0, MAX_LINE);
	}
}