

#include "pch.h"
#pragma comment (lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
int main(/*int argc, char *argv[]*/)
{

	WSADATA wsa;
	SOCKET client;
	SOCKADDR_IN server_addr;

	WSAStartup(MAKEWORD(2, 2), &wsa);
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi("88"));
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(client, (SOCKADDR *)&server_addr, sizeof(server_addr));

	/*//computer name
	char buff[256];
	DWORD nSize;
	GetComputerNameA(buff, &nSize);
	printf("\nName: %s", buff);


	//computer drives
	char buff_drives[256];
	DWORD length = GetLogicalDriveStringsA(sizeof(buff_drives), buff_drives);
	printf("number of drives: %d", length);
	printf("\nLogical drives: %s", buff_drives);*/

	char buf[256];
	int offset = 0;
	
	char computerName[256];
	DWORD nSize;
	GetComputerNameA(computerName, &nSize);
	memcpy(buf, computerName, strlen(computerName) + 1);
	offset += strlen(computerName) + 1;

	char driveString[256];
	DWORD len = GetLogicalDriveStringsA(sizeof(driveString), driveString);
	printf("Drive String: %s", driveString);
	int numDriver = len / 4;

	memcpy(buf + offset, &numDriver, sizeof(numDriver));
	offset += sizeof(numDriver);

	DWORD sectorsPerCluster;
	DWORD bytesPerSector;
	DWORD numberOfFreeClusters;
	DWORD totalNumberOfClusters;
	double size;

	for (int i = 0; i < numDriver; i++)
	{
		GetDiskFreeSpaceA(driveString + i * 4, &sectorsPerCluster, &bytesPerSector,
			&numberOfFreeClusters, &totalNumberOfClusters);
		size = (double)sectorsPerCluster * (double)bytesPerSector *
			(double)totalNumberOfClusters / (1 << 30);

		memcpy(buf + offset, driveString + i * 4, 1);
		offset += 1;

		memcpy(buf + offset, &size, sizeof(size));
		offset += sizeof(size);
	}

	send(client, buf, offset, 0);

	system("PAUSE");
	closesocket(client);
	WSACleanup();


}
