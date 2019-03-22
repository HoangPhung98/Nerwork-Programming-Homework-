

#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

int main(int argc, char *argv[])
{
	WSADATA wsa;
	SOCKET client;
	SOCKADDR_IN server_addr;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(88);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int ret = connect(client, (SOCKADDR *)&server_addr, sizeof(server_addr));
	if (ret == 0) {
		printf("\nConnect to server successfully!");
		const char *xinchao = "Xin chao from client";
		char buff[256];
		sprintf(buff, "%s: %s", xinchao, argv[1]);
		while (true) {
			send(client, buff, strlen(buff), 0);
		}
		printf("sprintf done: %s", buff);
		
	}
	else {
		printf("\nConnect to server fail!");
	}
	system("PAUSE");
	closesocket(client);
	WSACleanup();
}

