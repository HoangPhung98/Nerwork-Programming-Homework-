
#include "pch.h"
#include <iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>

int main()
{
	WSADATA wsa;
	SOCKADDR_IN server_addr;
	SOCKET client;
	int startupResult;
	startupResult = WSAStartup(MAKEWORD(2, 2), &wsa);

	
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(9000);
	server_addr.sin_family = AF_INET;

	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect(client, (SOCKADDR *)&server_addr, sizeof(server_addr)) == 0) {
			const char *msg = "GET /mp3";
			send(client, msg, strlen(msg),0);
		printf("connected!");
	}
	else {
		int err = GetLastError();
		printf("err: %d", err);
	}

	system("PAUSE");
	
	/*if (startupResult) printf("Startup false!\n");
	else printf("Startup Successfully!\n");

	addrinfo *result;
	int getaddrinfoResult = getaddrinfo("google.com","https",NULL,&result);
	if (getaddrinfoResult) printf("Get address info false!\n");
	else {
		printf("Get address info successfully!\n");
		memcpy(&addr, result->ai_addr, result->ai_addrlen);
		printf("\nIP: %s", inet_ntoa(addr.sin_addr));
		printf("\nIP: %d", addr.sin_addr);
		printf("\nIP in 32bit unsign long integer: %d", inet_addr(inet_ntoa(addr.sin_addr)));
	}*/

	WSACleanup();
	return 0;
}
