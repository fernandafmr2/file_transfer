#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "env.h"

void send_file(FILE *fp, int sockfd);
ssize_t total = 0;

int main(int argc, char* argv[])
{
	int fd, ret;
	char *filename;
	FILE *fp;
	struct sockaddr_in serveraddr;

	if (argc != 3) {
		perror("usage:send_file filepath <Address>");
		exit(1);
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("can't allocate sockfd");
		exit(1);
	}  

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, argv[2], &serveraddr.sin_addr) < 0) {
		perror("Ipaddr convert error");
		exit(1);
	}

	ret = connect(fd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
	if (ret < 0) {
		perror("connect error");
		exit(1);
	}

	filename = basename(argv[1]);
	if (filename == NULL) {
		perror("cant get filename");
		exit(1);
	}

	char buff[BUFFSIZE] = {0};
	strncpy(buff, filename, strlen(filename));
	if (send(fd, buff, BUFFSIZE, 0) < 0) {
		perror("Can't send filename");
		exit(1);
	}

	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		perror("cant open file");
		exit(1);
	}

	send_file(fp, fd);
	printf("send success, numbytes = %ld\n", total);
	
	fclose(fp);
	close(fd);
	return 0;
}


void send_file(FILE *fp, int sockfd)
{
	int n;
	char sendline[MAX_LINE] = {0};
	while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) {
		total += n;
		if (n != MAX_LINE && ferror(fp)) {
			perror("read file error");
			exit(1);
		}

		if (send(sockfd, sendline, n, 0) < 0) {
			perror("cant send file");
			exit(1);
		}
		memset(sendline, 0, MAX_LINE);
	}
}