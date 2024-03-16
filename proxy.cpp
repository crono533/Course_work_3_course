#include <iostream>
#include <WinSock2.h>


//чтобы компилить g++ proxy.cpp -o proxy -lws2_32

#pragma comment(lib, "ws2_32.lib")

#define PROXY_PORT 51132

#define SERVER_IP "127.0.0.1"

#define SERVER_PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET proxySocket, serverSocket, clientSocket;
    struct sockaddr_in proxyAddr, serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // Create proxy socket
    proxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (proxySocket == INVALID_SOCKET) {
        std::cerr << "Error creating proxy socket\n";
        WSACleanup();
        return 1;
    }

    // Fill in proxy address structure
    proxyAddr.sin_family = AF_INET;
    proxyAddr.sin_port = htons(PROXY_PORT);
    proxyAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind proxy socket
    if (bind(proxySocket, (struct sockaddr*)&proxyAddr, sizeof(proxyAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(proxySocket);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    if (listen(proxySocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(proxySocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Proxy server listening on port " << PROXY_PORT << std::endl;

    // Accept connections from clients
    clientSocket = accept(proxySocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed\n";
        closesocket(proxySocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client connected\n";

    // Connect to server
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating server socket\n";
        closesocket(proxySocket);
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed\n";
        closesocket(proxySocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server\n";

    // Forward data between client and server
char buffer[4096];
int bytesReceived, bytesSent;

while (true) {
    // Receive data from client
    bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) break; // Client disconnected or error

    // Log client message
    std::cout << "Received " << bytesReceived << " bytes from client." << std::endl;
    std::cout << "Client: " << std::string(buffer, bytesReceived) << std::endl;

    // Send data to server
    bytesSent = send(serverSocket, buffer, bytesReceived, 0);
    if (bytesSent <= 0) break; // Disconnect or error

    // Receive response from server
    bytesReceived = recv(serverSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) break; // Server disconnected or error

    // Log server response
    std::cout << "Received " << bytesReceived << " bytes from server." << std::endl;
    std::cout << "Server: " << std::string(buffer, bytesReceived) << std::endl;

    // Send response back to client
    bytesSent = send(clientSocket, buffer, bytesReceived, 0);
    if (bytesSent <= 0) break; // Disconnect or error
}

    // Cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    closesocket(proxySocket);
    WSACleanup();

    return 0;
}
