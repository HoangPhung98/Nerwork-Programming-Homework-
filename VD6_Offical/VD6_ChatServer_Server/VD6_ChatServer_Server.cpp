
#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

SOCKET clients[64];
char *ids[64];
int num_clients = 0;
bool login_success_clients[64];
int main()
{
	WSADATA wsa;
	SOCKET listener;
	SOCKADDR_IN server_addr;

	int ret_accep	t;

	WSAStartup(MAKEWORD(2, 2), &wsa);
	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(88);
	server_addr.sin_addr.s_addr = htonl(ADDR_ANY);

	bind(listener, (SOCKADDR *)&server_addr, sizeof(server_addr));
	listen(listener, 5);

	fd_set fdread;
	int ret;
	
	//select model
	char buff_sendToClient[256];
	const char *msg_login = "Please login: ";
	int ret_number_of_s;
	char buff_client_id[256];
	char buff_id[256];
	char buff_tmp[256];
	char buf[256];
	const char *msg_err = "Wrong login form, please do it again!\n";
	const char *msg_ok = "Login Successfully!\n";
	while (true)
	{
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);
		for (int i = 0; i < num_clients; i++) {
			FD_SET(clients[i], &fdread);
		}

		ret = select(0, &fdread, NULL, NULL, NULL);
		if (ret == SOCKET_ERROR) {
			printf("socket err\n");
			break;
		}

		if (ret > 0)
		{
			// Xu ly su kien co ket noi moi
			if (FD_ISSET(listener, &fdread))
			{
				SOCKET client = accept(listener, NULL, NULL);
				printf("New client accepted: %d\n", client);

				clients[num_clients] = client;
				num_clients++;

				send(client, msg_login, strlen(msg_login), 0);
			}

			for (int i = 0; i < num_clients; i++)
				if (FD_ISSET(clients[i], &fdread))
				{
					ret = recv(clients[i], buf, sizeof(buf), 0);
					buf[ret] = 0;

					if (!login_success_clients[i]) {
						
						printf("Received from %d: %s\n", clients[i], buf);
						//check login
						ret_number_of_s = sscanf(buf, "%s %s %s", buff_client_id, buff_id, buff_tmp);
						if (ret_number_of_s == 2) {
							if (strncmp(buff_client_id, "client_id:", 10) == 0) {
								login_success_clients[i] = true;
								continue;
							}
							else {
								send(clients[i], msg_err, strlen(msg_err), 0);
							}
						}
						else {
							send(clients[i], msg_err, strlen(msg_err), 0);
						}
					}
					else {
						printf("Received from %d: %s\n", clients[i], buf);
						//send to other clients
						for (int i = 0; i < num_clients; i++) {
							send(clients[i], buf, strlen(buf), 0);
						}

					}
					
				}
		}
	}

	return 0;

	closesocket(listener);
	WSACleanup();
}

DWORD WINAPI ClientThread(LPVOID lpParam) {
	SOCKET client = *(SOCKET *)lpParam;
	int ret_recvFromClient;
	char buff_recvFromClient[256];
	char buff_client_id[256];
	char buff_id[256];
	char buff_tmp[256];
	const char *msg_login = "login: ";
	int ret_number_of_s;

	send(client, msg_login, strlen(msg_login), 0);

	//check login information for the first time of client
	while (true) {
		const char *msg_err = "Wrong login form, please do it again!\n";
		const char *msg_ok = "Login Successfully!\n";

		ret_recvFromClient = recv(client, buff_recvFromClient, sizeof(buff_recvFromClient), 0);
		buff_recvFromClient[ret_recvFromClient] = 0;
		ret_number_of_s = sscanf(buff_recvFromClient, "%s %s %s", buff_client_id, buff_id, buff_tmp);
		if (ret_number_of_s == 2) {
			if (strncmp(buff_client_id, "client_id:", 10) == 0) {
				clients[num_clients++] = client;
				ids[num_clients - 1] = buff_id;
				printf("Accept new client: %s\n", ids[num_clients - 1]);
				break;
			}
			else {
				send(client, msg_err, strlen(msg_err), 0);
			}
		}
		else {
			send(client, msg_err, strlen(msg_err), 0);
		}

	}

	while (true) {
		ret_recvFromClient = recv(client, buff_recvFromClient, sizeof(buff_recvFromClient), 0);
		buff_recvFromClient[ret_recvFromClient] = 0;

		sprintf_s(buff_tmp, "%s: %s", buff_id, buff_recvFromClient);
		buff_tmp[strlen(buff_tmp)] = 0;
		for (int i = 0; i < num_clients; i++) {
			if (client != clients[i]) send(clients[i], buff_tmp, strlen(buff_tmp), 0);
		}
	}
}