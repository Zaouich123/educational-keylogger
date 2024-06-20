#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>

void sendLogFile(SOCKET clientSocket);
void startServer();
void handleClient(SOCKET clientSocket);

extern std::string resultPathAll;

#endif // SERVER_H