#include <iostream>
#include <WinSock2.h>

//чтобы компилить g++ proxy.cpp -o proxy -lws2_32

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define PORT 51132

#define PROXY_IP "192.168.137.1"

int main() {
    //объект струткры WSADATA (Windows Sockets Data), нужен для инициализации и контроля состояния сокетов
    //с помощью этой стрктуры можно понимать сколько зарезервировано памяти под сокеты и многое другое
    WSADATA wsaData; 



    // Initialize Winsock
    //при инициализации MAKEWORD дает понять какую версию WinSock запускать, выбрал 2.2 т.к. она новее просто
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    SOCKET clientSocket;  //объявляем сокет 

    //AF_INET указывает на семейтво проктоколов IPv4
    //SOCK_STREAM - тип сокета который поддерживает постоянное соединение
    //IPPROTO_TCP - явно указываем на использование TCP протокола
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Error creating socket\n";
        WSACleanup();
        return 1;
    }

    // Fill in proxy server address structure (устанавливаем соединение и заполняем переменную адресата с кем будем общаться)
    struct sockaddr_in proxyAddr;
    char buffer[1024];
    proxyAddr.sin_family = AF_INET; //устанавливаем семейство протоколов
    proxyAddr.sin_port = htons(PORT); //с помощью htons переводим номер порта в TCP/TP представлание
    proxyAddr.sin_addr.s_addr = inet_addr(PROXY_IP);// с помощью inet_addr переводим PROXY_IP в предствление TCP/IP

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&proxyAddr, sizeof(proxyAddr)) == SOCKET_ERROR) {
        std::cout << "Connection failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server\n";

    // Communication with server
    while (true) {
        // Send message to server
        std::cout << "Enter message for server: ";
        std::cin.getline(buffer, sizeof(buffer));
        int bytesSent = send(clientSocket, buffer, strlen(buffer), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cout << "Send failed\n";
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        // Receive message from server
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cout << "Receive failed\n";
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        if (bytesReceived == 0) {
            std::cout << "Server closed connection\n";
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Server: " << buffer << std::endl;
    }

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
