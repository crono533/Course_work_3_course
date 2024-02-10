#include <iostream>
#include <WinSock2.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define PORT 51132
#define SERVER_IP "192.168.137.1"
int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Error creating socket\n";
        WSACleanup();
        return 1;
    }

    // Fill in server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
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
