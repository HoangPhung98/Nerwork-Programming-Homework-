// ChatServer_Server.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TelnetServer_Server.h"
#pragma comment(lib, "ws2_32.lib")
#include<winsock2.h>
#include<WS2tcpip.h>
#include<string.h>
#include<stdio.h>
#define MAX_LOADSTRING 100
#define WM_SOCKET WM_USER + 1

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

SOCKET thisServer;
struct client {
	SOCKET client;
	char name[128];
	char password[128];
	bool isNameOK = false;
	bool isPasswordOK = false;
	bool isLogined = false;
}clients[64];
struct user {
	char name[128];
	char password[128];
}users[64];
int num_clients = 0;
int num_users = 0;
SOCKADDR_IN thisServer_addr;
WSADATA wsa;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int findIndexOfClient(SOCKET);
bool checkLoginForm(char[]);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TELNETSERVERSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	WSAStartup(MAKEWORD(2, 2), &wsa);
	thisServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	thisServer_addr.sin_family = AF_INET;
	thisServer_addr.sin_port = htons(88);
	thisServer_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(thisServer, (SOCKADDR *)&thisServer_addr, sizeof(thisServer_addr));
	listen(thisServer, 5);

	//read user file
	FILE *f = fopen("user.txt", "r");
	char buff_getALineInFile[128];
	while (fgets(buff_getALineInFile, sizeof(buff_getALineInFile), f)) {
		sscanf(buff_getALineInFile,"%s %s",users[num_users].name, users[num_users].password);
		num_users++;
	}
	fclose(f);
	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TELNETSERVERSERVER));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TELNETSERVERSERVER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TELNETSERVERSERVER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 400, 650, nullptr, nullptr, hInstance, nullptr);
	WSAAsyncSelect(thisServer, hWnd, WM_SOCKET, FD_ACCEPT | FD_READ);

	if (!hWnd)
	{
		return FALSE;
	}
	//design UI
	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL, 5, 5, 375, 610, hWnd,
		(HMENU)IDC_LIST_CLIENT, GetModuleHandle(NULL), NULL);

	const char *msg_accept = "Server";
	char buff_toSendAcceptedMsgToClient[64];
	sprintf(buff_toSendAcceptedMsgToClient, "%s : %d", msg_accept, thisServer);
	SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)buff_toSendAcceptedMsgToClient);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SOCKET:
	{
		if (WSAGETASYNCERROR(lParam)) {
			closesocket(thisServer);
			return -1;	
		}
		else {
			SOCKET newClient = NULL;
			switch (WSAGETSELECTEVENT(lParam)) {
			case FD_ACCEPT:
			{
				newClient = accept(thisServer, NULL, NULL);
				clients[num_clients++].client = newClient;

				char buff_toSendAcceptedMsgToClient[64];
				const char *msg_accept = "ACCEPTED new Client";
				const char *msg_requireLogin = "Please login \n";
				const char *msg_requireUser = "User: ";

				send((SOCKET)newClient, msg_requireLogin, strlen(msg_requireLogin), 0);
				send((SOCKET)newClient, msg_requireUser, strlen(msg_requireUser), 0);

				sprintf(buff_toSendAcceptedMsgToClient, "%s : %d", msg_accept, (SOCKET)newClient);
				SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)buff_toSendAcceptedMsgToClient);

				break;
			}
			case FD_READ:
			{
				int indexOfThisClient = findIndexOfClient((SOCKET)wParam);
				if (clients[indexOfThisClient].isLogined) {
					int ret_recvFromClient;
					char buff_recvFromClient[565];
					const char *msg_output_direction = "> out.txt";
					ret_recvFromClient = recv(wParam, buff_recvFromClient, sizeof(buff_recvFromClient), 0);
					buff_recvFromClient[ret_recvFromClient] = 0;
					strcat(buff_recvFromClient, msg_output_direction);
					system(buff_recvFromClient);

					char buff_readFileOut[565];
					FILE *f = fopen("out.txt", "r");
					while (fgets(buff_readFileOut, sizeof(buff_readFileOut), f)) {
						strcat(buff_readFileOut, "\n");
						for (int i = 0; i < num_clients; i++) {
							if (clients[i].isLogined /*&& clients[i].client != (SOCKET)wParam*/)
								send(clients[i].client, buff_readFileOut, strlen(buff_readFileOut), 0);
							SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)buff_readFileOut);

						}
					}
					fclose(f);
					char buff_toSendAcceptedMsgToClient[1024];
					sprintf(buff_toSendAcceptedMsgToClient, "%s : %s", clients[indexOfThisClient].name, buff_recvFromClient);
					SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)buff_toSendAcceptedMsgToClient);
					SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, WM_VSCROLL, SB_BOTTOM, 0);

				}
				else {
					char buff_toDisplay[1024];
					int ret_recvFromClient;
					char buff_recvFromClient[1024];

					ret_recvFromClient = recv((SOCKET)wParam, buff_recvFromClient, sizeof(buff_recvFromClient), 0);
					buff_recvFromClient[ret_recvFromClient] = 0;
					sprintf(buff_toDisplay, "%d : %s", wParam, buff_recvFromClient);
					SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)buff_toDisplay);
					SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, WM_VSCROLL, SB_BOTTOM, 0);
					//sscanf(buff_recvFromClient, "%s %s", buff_flag_requirement, buff_recvFromClient);

					if (!(clients[indexOfThisClient].isNameOK)) {
						bool foundUser = false;
						for(int i=0; i<num_users; i++)
							if (strcmp(users[i].name, buff_recvFromClient) == 0) {
								clients[indexOfThisClient].isNameOK = true;
								strcpy(clients[indexOfThisClient].name, buff_recvFromClient);

								const char *msg_loginContinues = "Password: ";
								send((SOCKET)wParam, msg_loginContinues, strlen(msg_loginContinues), 0);
								foundUser = true;
								break;
							}
						if (!foundUser) {
							const char *msg_loginFail = "Wrong user name, type again: ";
							send((SOCKET)wParam, msg_loginFail, strlen(msg_loginFail), 0);
						}
							
					}
					else if (!(clients[indexOfThisClient].isPasswordOK)) {
						bool foundPassword = false;
						for (int i = 0; i < num_users; i++) 
							if (strcmp(users[i].password, buff_recvFromClient) == 0) {
								clients[indexOfThisClient].isPasswordOK = true;
								clients[indexOfThisClient].isLogined = true;
								strcpy(clients[indexOfThisClient].password, buff_recvFromClient);

								const char *msg_loginSuces = "Login Succesfully! Now you can chat:\n";
								send((SOCKET)wParam, msg_loginSuces, strlen(msg_loginSuces), 0);
								foundPassword = true;
								break;
							}
						if (!foundPassword) {
							const char *msg_loginFail = "Wrong password, type again: ";
							send((SOCKET)wParam, msg_loginFail, strlen(msg_loginFail), 0);
						}
					
					}
				}
				break;
			}
			}

		}

		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
int findIndexOfClient(SOCKET client) {
	for (int i = 0; i < num_clients; i++)
		if (client == clients[i].client) return i;
	return -1;
}
bool checkLoginForm(char buff_loginMsg[1024]) {
	if (strncmp(buff_loginMsg, "id:", 3) == 0) {
		return true;
	}
	return false;
}
// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
