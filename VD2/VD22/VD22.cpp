
#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
int main()
{

	WSADATA wsa;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (ret == 0) printf("startup successfully");
	else printf("startup fail!");
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	SOCKADDR_IN server_addr;
	addrinfo *res;
	//domain resolution
	ret = getaddrinfo("genk.vn", "http", NULL, &res);
	if (ret == 0) {
		printf("\nAddress resolution successfully!");
		memcpy(&server_addr, res->ai_addr, res->ai_addrlen);
	}
	else {
		printf("\nAddress resolution fail!");
		return -1;
	}

	//connect to server

	ret = connect(client, (SOCKADDR *)&server_addr, sizeof(server_addr));
	if (ret == 0) {
		printf("\nConnect successfully!");
		const char *msg= "GET / HTTP/1.1\r\nHost: genk.vn\r\n\r\n";
		send(client, msg, strlen(msg), 0);
		
		//receive data from server
		char buff[1024];
		while (true) {
			ret = recv(client, buff, sizeof(buff), 0);
			if (ret <= 0) break;
			if (ret < 1024) buff[ret] = 0;
			printf("\n**************recv: %s", buff);
		}
	}
	else {
		printf("\nConnect Fail!");
		return -1;
	}

	WSACleanup();
	return 0;

}
