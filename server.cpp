#include <iostream>
#include <WinSock2.h>

//чтобы компилить g++ proxy.cpp -o proxy -lws2_32

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, proxySocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);
    char buffer[1024];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Error creating socket\n";
        WSACleanup();
        return 1;
    }

    // Bind socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Get local IP address and port
    getsockname(serverSocket, (struct sockaddr*)&serverAddr, &addrLen);
    char* localIP = inet_ntoa(serverAddr.sin_addr);
    u_short localPort = ntohs(serverAddr.sin_port);

    std::cout << "Server listening on " << localIP << ":" << localPort << std::endl;

    // Accept connection
    proxySocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (proxySocket == INVALID_SOCKET) {
        std::cout << "Accept failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client connected\n";

    // Communication with client
    while (true) {
        // Receive message from client
        int bytesReceived = recv(proxySocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cout << "Receive failed\n";
            closesocket(proxySocket);
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }
        if (bytesReceived == 0) {
            std::cout << "Client disconnected\n";
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Client: " << buffer << std::endl;

        // Send message to client
        std::cout << "Enter message for client: ";
        std::cin.getline(buffer, sizeof(buffer));
        int bytesSent = send(proxySocket, buffer, strlen(buffer), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cout << "Send failed\n";
            closesocket(proxySocket);
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }
    }

    // Cleanup
    closesocket(proxySocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
