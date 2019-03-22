
#include "pch.h"
#pragma comment(lib,"ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#include<winsock2.h>
#include<WS2tcpip.h>
#include <iostream>

DWORD WINAPI ListenerThread(LPVOID);
//void readFileUser(char **[], char **[]);
SOCKET clients[64];
int num_clients = 0;
int num_users = 0;
char *users[64];
char *password[64];

int main()
{
	//init network
	WSADATA wsa;
	SOCKADDR_IN thisServer_addr;
	SOCKET server;

	WSAStartup(MAKEWORD(2, 2), &wsa);
	thisServer_addr.sin_family = AF_INET;
	thisServer_addr.sin_port = htons(88);
	thisServer_addr.sin_addr.s_addr = htonl(ADDR_ANY);

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(server, (SOCKADDR *)&thisServer_addr, sizeof(thisServer_addr));
	listen(server, 5);
	//read file user info
	char buff[256];
	char buff_user[128];
	char buff_pass[128];
	FILE *f = fopen("user.txt", "r");
	while (fgets(buff, sizeof(buff), f) != NULL) {

		sscanf(buff, "%s %s", buff_user, buff_pass);
		users[num_users] =  new char[128];
		strcpy(users[num_users], buff_user);
		password[num_users] = new char[128];
		strcpy(password[num_users], buff_pass);

		num_users++;
	}
	fclose(f);

	//recv new client connection
	int ret_recvNewConnection;
	const char *msg_erro_accept = "Error! Can't connect to server!\n";
	const char *msg_acceptConnection = "Accepted connection from";
	while (true) {
		SOCKET newClient = accept(server, NULL, NULL);
		if (newClient != INVALID_SOCKET) {
			printf("%s: %d\n",msg_acceptConnection, newClient);
			CreateThread(0, 0, ListenerThread, &newClient, 0, 0);
		}
	}

	closesocket(server);
	WSACleanup();

}
DWORD WINAPI ListenerThread(LPVOID lpParam) {
	SOCKET client = *(SOCKET *)lpParam;
	clients[num_clients] = client;
	const char *msg_loginRequirment_user = "Please login:\nUser: ";
	const char *msg_loginRequirment_pass = "Please type password:\nPassword: ";

	const char *msg_UserOk = "USER_OK";
	const char *msg_PassOk = "PASS_OK";

	const char *msg_WrongUser = "Wrong user, please resend!\nUser: ";
	const char *msg_WrongPass = "Wrong password, please resend!\nPassword: ";


	//require user send user info
	bool check_found;
	int ret_recvFromClient;
	char buff_recvFromClient[256];
	send(client, msg_loginRequirment_user, strlen(msg_loginRequirment_user), 0);
	check_found = false;
	while (true) {
		ret_recvFromClient = recv(client, buff_recvFromClient, sizeof(buff_recvFromClient), 0);
		buff_recvFromClient[ret_recvFromClient] = 0;
		for (int i = 0; i < num_users; i++) {
			if (strcmp(buff_recvFromClient, users[i]) == 0) {
				check_found = true;
				break;
			}
		}
		if (check_found) {
			send(client, msg_UserOk, strlen(msg_UserOk), 0);
			break;
		}else{
			send(client, msg_WrongUser, strlen(msg_WrongUser), 0);
		}
		printf("%s\n", buff_recvFromClient);
	}
	//require password
	send(client, msg_loginRequirment_pass, strlen(msg_loginRequirment_pass), 0);
	check_found = false;
	while (true) {
		ret_recvFromClient = recv(client, buff_recvFromClient, sizeof(buff_recvFromClient), 0);
		buff_recvFromClient[ret_recvFromClient] = 0;
		for (int i = 0; i < num_users; i++) {
			if (strcmp(buff_recvFromClient, password[i]) == 0) {
				check_found = true;
				break;
			}
		}
		if (check_found) {
			send(client, msg_PassOk, strlen(msg_PassOk), 0);
			break;
		}
		else {
			send(client, msg_WrongPass, strlen(msg_WrongPass), 0);
		}
		printf("recv: %s\n", buff_recvFromClient);
	}
	//recv cmd from client
	const char *msg_allow_cmd = "You can type cmd now: \n";
	const char *fileDirection = "> .\\output2.txt";
	char buff_cmd[256];
	char buff_readFile[256];
	int ret_recv_cmd;

	send(client,msg_allow_cmd, strlen(msg_allow_cmd), 0);
	while (true) {
		ret_recv_cmd = recv(client, buff_recvFromClient, sizeof(buff_recvFromClient), 0);
		buff_recvFromClient[ret_recv_cmd] = 0;
		sprintf_s(buff_cmd, "%s %s", buff_recvFromClient, fileDirection);
		system(buff_cmd);

		//send result to all file
		FILE *f = fopen("output2.txt", "r");
		while (fgets(buff_readFile, sizeof(buff_readFile), f) != NULL) {
			printf(buff_readFile);
			send(client, buff_readFile, strlen(buff_readFile),0);
		}
		fclose(f);
	}
}