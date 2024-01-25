#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <thread>
#include <vector>
#include <memory>
#pragma comment(lib, "ws2_32.lib")

std::vector<std::thread*> ThreadPool;
int main()
{
	ThreadPool.reserve(100);
	for (int i = 0; i < 10; i++)
	{
		std::thread* NewThread = new std::thread();
		ThreadPool.push_back(NewThread);
	}

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServSock = 0;
	ServSock = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServAddr;
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = INADDR_ANY;
	ServAddr.sin_port = htons(10000);

	unsigned long a = htonl(000001);
	bind(ServSock, (SOCKADDR*)&ServAddr, sizeof(ServAddr));
	// 서버용 소켓을 만들었다. 이 소켓은 ipv4 형식이며 어떤 ip와도 통신할수있으며 포트번호는 10000이다.


	listen(ServSock, 10);

	SOCKET NewSock;
	NewSock = accept(ServSock, nullptr, nullptr);

	char RecvMsg[1000];
	int resvBite = recv(NewSock, RecvMsg, sizeof(RecvMsg), 0);
	RecvMsg[resvBite] = '\0';
	std::cerr << RecvMsg;
	closesocket(NewSock);
	closesocket(ServSock);

	WSACleanup();
}
