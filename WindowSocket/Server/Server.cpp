#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET ServSock = socket(PF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN ServAddr;
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = INADDR_ANY;
    ServAddr.sin_port = htons(10000);

    bind(ServSock, (SOCKADDR*)&ServAddr, sizeof(ServAddr));
    listen(ServSock, 10);

    std::cout << "Server listening...\n";

    SOCKET NewSock;
    NewSock = accept(ServSock, nullptr, nullptr);

    while (true)
    {
        char RecvMsg[1000];
        int resvByte = recv(NewSock, RecvMsg, sizeof(RecvMsg), 0);
        RecvMsg[resvByte] = '\0';
        std::cout << "Received: " << RecvMsg << std::endl;
        send(NewSock, RecvMsg, sizeof(RecvMsg), 0);
    }

    closesocket(NewSock);
    closesocket(ServSock);
    WSACleanup();
    return 0;
}
