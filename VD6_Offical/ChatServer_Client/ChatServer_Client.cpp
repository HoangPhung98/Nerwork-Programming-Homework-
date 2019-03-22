
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
	fd_set fdread;
	fd_set fdwrite;
	int ret;
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	WSAStartup(MAKEWORD(2, 2), &wsa);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(88);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ret = connect(client, (SOCKADDR *)&server_addr, sizeof(server_addr));

	char buf[256];
	char buff_sendToServer[256];

	while (true)
	{
		FD_ZERO(&fdread);
		FD_SET(client, &fdread);

		gets_s(buff_sendToServer, sizeof(buff_sendToServer));
		buff_sendToServer[strlen(buff_sendToServer)] = 0;
		send(client, buff_sendToServer, strlen(buff_sendToServer), 0);
		printf("Send: %s\n", buff_sendToServer);

		ret = select(0, &fdread, NULL, NULL, &tv);
		if (ret == SOCKET_ERROR)
		{
			break;
		}
		if (ret == 0)
		{
			printf("Timed Out!\n");
		}
		else if (ret > 0)
		{
			if (FD_ISSET(client, &fdread)) {
				ret = recv(client, buf, sizeof(buf), 0);
				buf[ret] = 0;
				printf("Received: %s\n", buf);

			}	
				
			/*gets_s(buff_sendToServer, sizeof(buff_sendToServer));
			buff_sendToServer[strlen(buff_sendToServer)] = 0;
			send(client, buff_sendToServer, strlen(buff_sendToServer), 0);*/
		}
	}
	closesocket(client);
	WSACleanup();
	return 0;
}
