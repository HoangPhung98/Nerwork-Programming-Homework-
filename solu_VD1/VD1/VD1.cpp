#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "winsock2.h"
#include "ws2tcpip.h"

int main() {

	WSADATA wsa;
	int startup_result;
	startup_result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (startup_result == 0) printf("Startup successfully!\n"); else printf("Startup fails!\n");

	//address resolution
	addrinfo *result;
	sockaddr_in addr;
	int check_result;
	check_result = getaddrinfo("genk.vn", "http", NULL, &result);
	if (check_result == 0) {
		printf("Resolute domain name successfully!\n");
		memcpy(&addr, result->ai_addr, result->ai_addrlen);
		printf("IP: %s", inet_ntoa(addr.sin_addr));
	}
	else {
		printf("Resolute domain name fail\n");
	}
	WSACleanup();
}

/*int main()
{
	WSADATA wsa;
	WORD version = MAKEWORD(2, 2);

	if (WSAStartup(version, &wsa) == 0)
	{
		printf("Khoi tao thanh cong\n");
	}
	else
	{
		printf("Khoi tao that bai\n");
	}

	addrinfo * result;
	sockaddr_in addr;

	int res = getaddrinfo("genk.vn", "http", NULL, &result);
	if (res == 0)
	{
		printf("Phan giai thanh cong\n");
		memcpy(&addr, result->ai_addr, result->ai_addrlen);

		printf("IP: %s", inet_ntoa(addr.sin_addr));
	}

	system("PAUSE");
	return 0;
}*/