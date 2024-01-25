#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(10000);
	inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr));

	connect(ClientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	const char* msg = "sdsd";
	send(ClientSocket, msg, strlen(msg), 0);
	closesocket(ClientSocket);
	WSACleanup();
}
