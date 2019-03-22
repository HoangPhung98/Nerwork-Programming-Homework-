
#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
int main()
{
	// Khoi tao Winsock
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// Khoi tao socket
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Khai bao dia chi server
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);


	int result_connect = connect(client, (SOCKADDR *)&addr, sizeof(addr));
	if (result_connect == 0) {

		printf("\n connect successfully!");
		//int result_recv;
		const char *msg = "Hello server. This is a new client.";
		send(client, msg, strlen(msg), 0);

		char buff[5];
		while (true) {

			result_connect = recv(client, buff, sizeof(buff), 0);
			printf("\nrecv : %d", result_connect);
			if (result_connect <= 0) break;

			if (result_connect < 5) buff[result_connect] = 0;

			printf(buff);


			if (strncmp(buff, "exit", 4)==0) break;

		}
	}
	else {
		printf("\nconnect fail!");
		result_connect = WSAGetLastError();
		printf("error: %d", result_connect);
		return 1;
	}

	closesocket(client);
	WSACleanup();
	system("PAUSE");
	return 0;
}

/*#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	/*if (result_startup) {
		printf("Startup false!");
		return 1;
	}
	else printf("Startup successfully");

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN netcat_addr;

	netcat_addr.sin_family = AF_INET;
	netcat_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	netcat_addr.sin_port = htons(9000);


	int result_connect = connect(client, (SOCKADDR *)&netcat_addr, sizeof(netcat_addr));
	if (result_connect == 0) {

		printf("\n connect successfully!");
		//int result_recv;
		const char *msg = "Hello server. This is a new client.";
		send(client, msg, strlen(msg), 0);

		char buff[5];
		while (true) {

			result_connect = recv(client, buff, sizeof(buff), 0);
			//printf("\nrecv : %d", result_recv);
			if (result_connect <= 0) break;

			if (result_connect < 5) buff[result_connect] = 0;

			printf(buff);


			if (strncmp(buff, "exit", 4)) break;

		}
	}else {
		printf("\nconnect fail!");
		result_connect = WSAGetLastError();
		printf("error: %d", result_connect);
		return 1;
	}

	closesocket(client);
	WSACleanup();
	system("PAUSE");
	return 0;
}
*/