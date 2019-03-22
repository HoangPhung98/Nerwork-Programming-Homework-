
#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
DWORD WINAPI ReceiverThread(LPVOID);
int main()
{
	WSADATA wsa;
	SOCKET client;
	SOCKADDR_IN server_addr;
	int ret;

	WSAStartup(MAKEWORD(2, 2), &wsa);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(88);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ret = connect(client, (SOCKADDR *)&server_addr, sizeof(server_addr));
	if (ret == 0) {
		printf("Connect to server Successfully!\n");
		CreateThread(0,0, ReceiverThread, &client, 0, 0);
		char buff_sendToServer[256];
		while (true) {

			gets_s(buff_sendToServer, sizeof(buff_sendToServer));
			buff_sendToServer[strlen(buff_sendToServer)] = 0;
			send(client, buff_sendToServer, strlen(buff_sendToServer),0);
		}
	}
	else {
		printf("Connect to server Fail!\n");
	}
	closesocket(client);
	WSACleanup();

}
DWORD WINAPI ReceiverThread(LPVOID lpParam) {
	SOCKET client = *(SOCKET *)lpParam;
	char buff_recvFromServer[256];
	int ret;

	while (true) {
		ret = recv(client, buff_recvFromServer, sizeof(buff_recvFromServer), 0);
		buff_recvFromServer[ret] = 0;
		printf("%s\n", buff_recvFromServer);
	}
	/*while (true) {
		ret = recv(client, buff_recvFromServer, sizeof(buff_recvFromServer), 0);
		buff_recvFromServer[ret] = 0;
		printf(buff_recvFromServer);
	}*/
}