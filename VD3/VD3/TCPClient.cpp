
#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")


int main(/*int argc, char *argv[]*/)
{
	WSADATA wsa;
	SOCKET client;
	SOCKADDR_IN server_addr;

	WSAStartup(MAKEWORD(2,2), &wsa);

	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi("88"));
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int ret = connect(client, (SOCKADDR *)&server_addr, sizeof(server_addr));
	if (ret == 0) {
		printf("Connect to server successfully!");

		//recv hello from server
		char buff_msgrecv[256];
		ret = recv(client, buff_msgrecv, strlen(buff_msgrecv), 0);
		buff_msgrecv[ret] = 0;
		printf("\n%s",buff_msgrecv);

		char buff_msgtosend[256];
		while (true) {
			
			printf("\nType message:");
			gets_s(buff_msgtosend, sizeof(buff_msgtosend));
			send(client, buff_msgtosend, strlen(buff_msgtosend), 0);
			if (strncmp(buff_msgtosend, "exit", 4) == 0) break;

			ret = recv(client, buff_msgrecv, strlen(buff_msgrecv), 0);
			if (ret <= 0) break;
			if (ret < 256) buff_msgrecv[ret] = 0;
			printf(buff_msgrecv);
		}
	}
	else {
		int err = GetLastError();
		printf("err: %d", err);
	}
}
