#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

int main() 
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(10000);
    inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr));

    connect(ClientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    while (true) 
    {
        std::string msg;
        std::cout << "Enter message: ";
        std::getline(std::cin, msg);
        if (msg.empty())
        {
            continue;
        }
        send(ClientSocket, msg.c_str(), msg.size(), 0);

        char recvMsg[1024]{};
        int MsgByte = recv(ClientSocket, recvMsg, sizeof(recvMsg), 0);
        if (MsgByte == -1)
        {
            continue;
        }
        recvMsg[MsgByte] = '\0';
        std::cout << "Server response: " << recvMsg << std::endl;
    }

    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}