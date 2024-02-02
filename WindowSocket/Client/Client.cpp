#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#pragma comment(lib, "ws2_32.lib")

constexpr int MaxBufferSize = 1024;
constexpr int Port = 54000;
constexpr char ServerIp[] = "127.0.0.1"; // 서버 IP 주소

std::queue<std::string> messageQueue;
std::mutex mtx;
std::condition_variable cv;

void ReceiveMessages(SOCKET serverSocket)
{
    char buffer[MaxBufferSize];
    while (true)
    {
        int bytesReceived = recv(serverSocket, buffer, MaxBufferSize, 0);
        if (bytesReceived <= 0)
        {
            std::cout << "Disconnected from server." << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        std::string message(buffer);
        std::lock_guard<std::mutex> lock(mtx);
        messageQueue.push(message);
        cv.notify_one();
    }
}

void ProcessMessagesFromQueue()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !messageQueue.empty(); });

        // Process messages from the queue
        std::string message = messageQueue.front();
        messageQueue.pop();
        std::cout << message << std::endl;
    }
}

int main()
{
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        std::cerr << "Can't initialize Winsock! Quitting" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Can't create socket! Quitting" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(Port);
    inet_pton(AF_INET, ServerIp, &hint.sin_addr);

    int connResult = connect(serverSocket, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        std::cerr << "Can't connect to server! Quitting" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    std::cout << "닉네임을 입력하세요 : ";
    std::string NickName;
    std::cin >> NickName;

    send(serverSocket, NickName.c_str(), NickName.size(), 0);

    std::thread recvThread(ReceiveMessages, serverSocket);
    std::thread processThread(ProcessMessagesFromQueue);

    std::string userInput;
    do
    {
        std::getline(std::cin, userInput);
        std::string msg = NickName + " : " + userInput;
        if (userInput.size() > 0)
        {
            int sendResult = send(serverSocket, msg.c_str(), msg.size(), 0);
            if (sendResult == SOCKET_ERROR)
            {
                std::cerr << "Failed to send message to server!" << std::endl;
                break;
            }
        }
    } while (userInput != "exit");



    recvThread.detach();
    processThread.detach();
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
