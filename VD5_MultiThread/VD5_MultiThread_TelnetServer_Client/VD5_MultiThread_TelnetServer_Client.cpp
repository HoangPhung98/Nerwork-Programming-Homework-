

#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
DWORD WINAPI ReceiverThread(LPVOID);


int main()
{
	//init network
	WSADATA wsa;
	SOCKET client;
	SOCKADDR_IN server_addr;

	WSAStartup(MAKEWORD(2, 2), &wsa);
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(88);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//
	connect(client, (SOCKADDR *)&server_addr, sizeof(server_addr));
	CreateThread(0, 0, ReceiverThread, &client, 0, 0);

	//***can always send data to server in main thread; 
	char buff_keyboard[256];
	while (true) {
		gets_s(buff_keyboard, sizeof(buff_keyboard));
		send(client, buff_keyboard, strlen(buff_keyboard), 0);
	}

	closesocket(client);
	WSACleanup();
}
DWORD WINAPI ReceiverThread(LPVOID lpParam) {
	//***new thread to recv data from server

	SOCKET server_recv = *(SOCKET *)lpParam;
	//recv requirments
	int ret_recvFromServer_requirment;
	char buff_requirment[256];
	char buff_check_login[256];
	char buff_user_info[256];
	while (true) {
		ret_recvFromServer_requirment = recv(server_recv, buff_requirment, sizeof(buff_requirment), 0);
		buff_requirment[ret_recvFromServer_requirment] = 0;
		printf(buff_requirment);
		if (strncmp(buff_requirment, "USER_OK", 7) == 0) {
			//printf("Valid User!\n");
		}
	}
	
}