#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

void receiveLogFile(SOCKET clientSocket, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }

    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        outFile.write(buffer, bytesReceived);
    }

    outFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <command>" << std::endl;
        return 1;
    }

    WSADATA wsaData;
    SOCKET clientSocket = INVALID_SOCKET;
    sockaddr_in serverAddress;
    std::string host = argv[1];
    std::string command = argv[2];

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        WSACleanup();
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr);
    serverAddress.sin_port = htons(54000);

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Error connecting to server" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    send(clientSocket, command.c_str(), command.size(), 0);

    if (command == "getlog") {
        receiveLogFile(clientSocket, "downloaded_log_file.txt"); // Spécifiez le chemin du fichier de destination
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
