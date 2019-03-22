
#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")


int main()
{
	WSADATA wsa;
	SOCKET listener;
	SOCKADDR_IN this_server_addr;
	unsigned long ul = 1;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ioctlsocket(listener, FIONBIO, &ul);

	this_server_addr.sin_family = AF_INET;
	this_server_addr.sin_port = htons(88);
	this_server_addr.sin_addr.s_addr = htonl(ADDR_ANY);

	bind(listener, (SOCKADDR *)&this_server_addr, sizeof(this_server_addr));
	listen(listener, 5);

	SOCKET clients[64];
	int numClients = 0;
	int ret;
	while (true) {
		SOCKET newClient = accept(listener, NULL, NULL);
		if (newClient == INVALID_SOCKET) {
			ret = GetLastError();
			if (ret == WSAEWOULDBLOCK) {
				//printf("\nBlocking err!");
			}
			else {
				printf("\nOthers err!");
				break;
			}
		}
		else {
			printf("\nNew client accepted!");
			clients[numClients++] = newClient;
		}
		int ret_recv;
		char buff_recv[256];
		for (int i = 0; i < numClients; i++) {
			ret_recv = recv(clients[i], buff_recv, sizeof(buff_recv), 0);

			//if (ret_recv <= 0) break;
			if (ret_recv == SOCKET_ERROR)
			{
				ret_recv = WSAGetLastError();
			}
			else {
				(ret_recv < 256) ? buff_recv[ret_recv] = 0 : buff_recv[255] = 0;
				printf("revc %d", ret_recv);
				printf("\n%s", buff_recv);
			}

		}
	}
	closesocket(listener);
	WSACleanup();

}
