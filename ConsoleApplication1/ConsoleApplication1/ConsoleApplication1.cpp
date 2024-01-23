#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		std::cerr << "winsock2.2 라이브러리 초기화 실패.\n";
		return 1;
	}
	else
	{
		std::cerr << "winsock2.2 라이브러리 초기화 성공.\n";
	}

	SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cerr << "클라이언트 소켓 생성 실패.\n";
		WSACleanup();
		return 1;
	}
	else
	{
		std::cerr << "클라이언트 소켓 생성 성공.\n";
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr));
	serverAddr.sin_port = htons(49152);
	std::cerr << "서버주소정보 구조체 생성 성공.\n";

	if (connect(ClientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cerr << "Failed to connect to server.\n";
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

}
//WSADATA wsaData;
//if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//    std::cerr << "Failed to initialize Winsock.\n";
//    return 1;
//}

//// 소켓 생성
//SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//if (clientSocket == INVALID_SOCKET) {
//    std::cerr << "Failed to create client socket.\n";
//    WSACleanup();
//    return 1;
//}

//// 서버 주소 설정
//sockaddr_in serverAddr;
//serverAddr.sin_family = AF_INET;
//serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
//serverAddr.sin_port = htons(12345);

//// 서버에 연결
//if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
//    std::cerr << "Failed to connect to server.\n";
//    closesocket(clientSocket);
//    WSACleanup();
//    return 1;
//}

//std::cout << "Connected to server.\n";

//// 서버로 데이터 전송
//const char* message = "Hello from client!";
//send(clientSocket, message, strlen(message), 0);

//// 서버에서 응답 수신
//char buffer[1024];
//int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
//if (bytesRead > 0) {
//    buffer[bytesRead] = '\0';
//    std::cout << "Received response from server: " << buffer << "\n";
//}

//// 소켓 종료
//closesocket(clientSocket);
//WSACleanup();

//return 0;

