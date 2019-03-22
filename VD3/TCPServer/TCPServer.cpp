
#include "pch.h"
#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>


int main(/*int argc, char *argv[]*/) {
	WSADATA wsa;
	SOCKET server;
	SOCKADDR_IN server_addr;

	WSAStartup(MAKEWORD(2, 2), &wsa);
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(88);
	server_addr.sin_addr.s_addr = htonl(ADDR_ANY);

	bind(server, (SOCKADDR *)&server_addr, sizeof(server_addr));
	listen(server, 5);

	SOCKET newClient = accept(server, NULL, NULL);
	printf("new client connect");

	//read hello file then send to client
	char buff_tosendClient[256];
	FILE *f_hello = fopen("hl.txt", "r");
	fgets(buff_tosendClient, sizeof(buff_tosendClient), f_hello);
	fclose(f_hello);
	send(newClient, buff_tosendClient, strlen(buff_tosendClient), 0);

	char buff_recvfromClient[256];
	int ret;
	FILE *f_chat_output = fopen("chat_output.txt", "w");
	while (true) {
		ret = recv(newClient, buff_recvfromClient, sizeof(buff_recvfromClient), 0);
		if (ret <= 0) break;
		buff_recvfromClient[ret] = 0;
		printf("\n%s", buff_recvfromClient);
		//fprintf(f_chat_output, "\n%s",buff_recvfromClient);
	}
	fclose(f_chat_output);
	system("PAUSE");
	closesocket(server);
	closesocket(newClient);
	WSACleanup();
}
