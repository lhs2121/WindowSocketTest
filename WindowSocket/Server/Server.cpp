#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#pragma comment(lib, "ws2_32.lib")

void AcceptClient();
void RegisterClient(SOCKET NewSocket);
void RecvMessage(SOCKET clientSocket);
void ProcessMessages();

struct Client
{
	SOCKET ClientSock;
	std::string NickName;
};

constexpr int MaxBufferSize = 1024;
constexpr int Port = 54000;

std::queue<std::string> messageQueue;
std::mutex mtx;
std::condition_variable cv;

SOCKET listeningSocket;
std::vector<Client> ClientGroup;

int main()
{
	// 라이브러리 초기화
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0) {
		std::cerr << "Can't initialize Winsock! Quitting" << std::endl;
		return 1;
	}

	// 소켓생성
	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listeningSocket == INVALID_SOCKET) {
		std::cerr << "Can't create socket! Quitting" << std::endl;
		WSACleanup();
		return 1;
	}

	// 소켓 구조체 설정
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(Port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	// 구조체를 소켓에 연결
	if (bind(listeningSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
	{
		std::cerr << "Can't bind socket! Quitting" << std::endl;
		closesocket(listeningSocket);
		WSACleanup();
		return 1;
	}

	// 수신가능상태로 변경
	if (listen(listeningSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cerr << "Can't listen on socket! Quitting" << std::endl;
		closesocket(listeningSocket);
		WSACleanup();
		return 1;
	}

	// 여기서부터 메세지큐 처리 시작합니다
	std::thread messageProcessor(ProcessMessages);

	// 소켓 받아서 처리하는 반복문
	AcceptClient();

	closesocket(listeningSocket);
	WSACleanup();

	return 0;
}

void AcceptClient()
{
	while (true)
	{
		SOCKET NewSocket = accept(listeningSocket, nullptr, nullptr);
		if (NewSocket == INVALID_SOCKET)
		{
			continue;
		}

		std::cout << "Client connected." << std::endl;
		RegisterClient(NewSocket);
	}
}

void RegisterClient(SOCKET NewSocket)
{
	char nickname[1024];
	int byte = recv(NewSocket, nickname, 1024, 0);
	nickname[byte] = '\0';

	Client NewClient;
	NewClient.ClientSock = NewSocket;
	NewClient.NickName = nickname;
	ClientGroup.push_back(NewClient);

	std::thread clientThread(RecvMessage, NewSocket);
	clientThread.detach();
}

void RecvMessage(SOCKET clientSocket)
{
	char buffer[MaxBufferSize];
	while (true)
	{
		int bytesReceived = recv(clientSocket, buffer, MaxBufferSize, 0);

		if (bytesReceived <= 0) {
			std::cout << "Client disconnected." << std::endl;
			break;
		}

		buffer[bytesReceived] = '\0';
		std::string message(buffer);

		std::lock_guard<std::mutex> lock(mtx);
		messageQueue.push(message);
		cv.notify_one();
	}
}

void ProcessMessages()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [] { return !messageQueue.empty(); });

		// Process messages from the queue
		std::string message = messageQueue.front();
		messageQueue.pop();

		for (size_t i = 0; i < ClientGroup.size(); i++)
		{
			send(ClientGroup[i].ClientSock, message.c_str(), message.size(), 0);
		}


		std::cout << "Received message: " << message << std::endl;
	}
}