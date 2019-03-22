
#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>

int main()
{
	//var
	WSADATA wsa;
	SOCKET listener;
	SOCKADDR_IN server_addr;

	WSAStartup(MAKEWORD(2, 2), &wsa);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9000);
	server_addr.sin_addr.s_addr = htonl(ADDR_ANY);

	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (SOCKADDR *)&server_addr, sizeof(server_addr));
	listen(listener, 5);

	while (true) {
		SOCKET newClient = accept(listener, NULL, NULL);
		printf("\nnew socket accepted");
		char recvfromclient_buff[1024];
		int ret = recv(newClient, recvfromclient_buff, sizeof(recvfromclient_buff), 0);
		printf(recvfromclient_buff);
	


		if (strncmp(recvfromclient_buff, "GET /mp3", 8) == 0) {
			printf("\ngetting");
			//header
			const char *msg = "HTTP/1.1 200 OK\r\nContent-Type: audio/mp3\r\n\r\n";
			send(newClient, msg, strlen(msg), 0);
			//content
			FILE *f = fopen("C:\\Users\\Hoang Phung\\source\\repos\\VD2\\VD23\\till_i_collapse_remix.mp3", "rb");
			if (f != NULL) {
				char sendtoclient_buff[1024];
				while (true) {
					ret = fread(sendtoclient_buff, 1, sizeof(sendtoclient_buff), f);
					send(newClient, sendtoclient_buff, ret, 0);
					if (ret <= 0) break;
				}
			}
		}else if (strncmp(recvfromclient_buff, "GET /image", 10) == 0) {
			printf("\ngetting image");
			//header
			const char *msg = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n";
			send(newClient, msg, strlen(msg), 0);
			//content
			FILE *f = fopen("C:\\Users\\Hoang Phung\\source\\repos\\VD2\\VD23\\silliconValley.jpg", "rb");
			if (f != NULL) {
				char sendtoclient_buff[1024];
				while (true) {
					ret = fread(sendtoclient_buff, 1, sizeof(sendtoclient_buff), f);
					send(newClient, sendtoclient_buff, ret, 0);
					if (ret <= 0) break;
				}
			}
		}
		else
		{
			// Tra ve du lieu text html
			const char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>Yeu cau khong duoc ho tro</body></html>";
			send(newClient, msg, strlen(msg), 0);
		}


		closesocket(newClient);
	}

}
