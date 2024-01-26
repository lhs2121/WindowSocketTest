﻿#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <string>
#pragma comment(lib, "ws2_32.lib")

void clientHandler(SOCKET clientSocket) 
{
    char recvMsg[1024];
    while (true) {
        int MsgByte = recv(clientSocket, recvMsg, sizeof(recvMsg), 0);
        if (MsgByte > 0) {
            recvMsg[MsgByte] = '\0';
            std::cout << "Received from client: " << recvMsg << std::endl;

            // Echo back to client
            send(clientSocket, recvMsg, MsgByte, 0);
        }
        else if (MsgByte == 0) {
            std::cerr << "Client disconnected" << std::endl;
            break;
        }
        else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }
    closesocket(clientSocket);
}

int main() 
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN ServAddr;
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = INADDR_ANY;
    ServAddr.sin_port = htons(10000);

    bind(ServSock, (SOCKADDR*)&ServAddr, sizeof(ServAddr));
    listen(ServSock, 10);

    std::cout << "Server listening..." << std::endl;

    std::vector<std::thread> threads;
    while (true) 
    {
        SOCKET NewSock;
        NewSock = accept(ServSock, nullptr, nullptr);
        std::cout << "New client connected" << std::endl;
        threads.emplace_back(clientHandler, NewSock);
    }

    closesocket(ServSock);
    WSACleanup();
    return 0;
}